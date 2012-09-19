/* spek-pipeline.c
 *
 * Copyright (C) 2010-2012  Alexander Kojevnikov <alexander@kojevnikov.com>
 *
 * Spek is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Spek is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Spek.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Conversion of decoded samples into an FFT-happy format is heavily
 * influenced by GstSpectrum which is part of gst-plugins-good.
 * The original code:
 * (c) 1999 Erik Walthinsen <omega@cse.ogi.edu>
 * (c) 2006 Stefan Kost <ensonic@users.sf.net>
 * (c) 2007-2009 Sebastian Dröge <sebastian.droege@collabora.co.uk>
 */

#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "spek-audio.h"
#include "spek-fft.h"

#include "spek-pipeline.h"

enum
{
    NFFT = 64 // Number of FFTs to pre-fetch.
};

struct spek_pipeline
{
    struct spek_audio_context *cx;
    const struct spek_audio_properties *properties;
    int bands;
    int samples;
    spek_pipeline_cb cb;
    void *cb_data;

    struct spek_fft_plan *fft;
    float *coss; // Pre-computed cos table.
    int nfft; // Size of the FFT transform.
    int input_size;
    int input_pos;
    float *input;
    float *output;

    pthread_t reader_thread;
    bool has_reader_thread;
    pthread_mutex_t reader_mutex;
    bool has_reader_mutex;
    pthread_cond_t reader_cond;
    bool has_reader_cond;
    pthread_t worker_thread;
    bool has_worker_thread;
    pthread_mutex_t worker_mutex;
    bool has_worker_mutex;
    pthread_cond_t worker_cond;
    bool has_worker_cond;
    bool worker_done;
    volatile bool quit;
};

// Forward declarations.
static void * reader_func(void *);
static void * worker_func(void *);
static void reader_sync(struct spek_pipeline *p, int pos);
static float average_input(const struct spek_pipeline *p, void *buffer);

struct spek_pipeline * spek_pipeline_open(
    const char *path, int bands, int samples, spek_pipeline_cb cb, void *cb_data)
{
    struct spek_pipeline *p = malloc(sizeof(struct spek_pipeline));
    p->cx = spek_audio_open(path);
    p->properties = spek_audio_get_properties(p->cx);
    p->bands = bands;
    p->samples = samples;
    p->cb = cb;
    p->cb_data = cb_data;

    p->coss = NULL;
    p->fft = NULL;
    p->input = NULL;
    p->output = NULL;
    p->has_reader_thread = false;
    p->has_reader_mutex = false;
    p->has_reader_cond = false;
    p->has_worker_thread = false;
    p->has_worker_mutex = false;
    p->has_worker_cond = false;

    if (!p->properties->error) {
        p->nfft = 2 * bands - 2;
        p->coss = malloc(p->nfft * sizeof(float));
        float cf = 2.0f * (float)M_PI / p->nfft;
        for (int i = 0; i < p->nfft; ++i) {
            p->coss[i] = cosf(cf * i);
        }
        p->fft = spek_fft_plan_new(p->nfft);
        p->input_size = p->nfft * (NFFT * 2 + 1);
        p->input = malloc(p->input_size * sizeof(float));
        p->output = malloc(bands * sizeof(float));
        spek_audio_start(p->cx, samples);
    }

    return p;
}

const struct spek_audio_properties * spek_pipeline_properties(struct spek_pipeline *pipeline)
{
    return pipeline->properties;
}

void spek_pipeline_start(struct spek_pipeline *p)
{
    if (p->properties->error) return;

    p->input_pos = 0;
    p->worker_done = false;
    p->quit = false;

    p->has_reader_mutex = !pthread_mutex_init(&p->reader_mutex, NULL);
    p->has_reader_cond = !pthread_cond_init(&p->reader_cond, NULL);
    p->has_worker_mutex = !pthread_mutex_init(&p->worker_mutex, NULL);
    p->has_worker_cond = !pthread_cond_init(&p->worker_cond, NULL);

    p->has_reader_thread = !pthread_create(&p->reader_thread, NULL, &reader_func, p);
    if (!p->has_reader_thread) {
        spek_pipeline_close(p);
    }
}

void spek_pipeline_close(struct spek_pipeline *p)
{
    if (p->has_reader_thread) {
        p->quit = true;
        pthread_join(p->reader_thread, NULL);
        p->has_reader_thread = false;
    }
    if (p->has_worker_cond) {
        pthread_cond_destroy(&p->worker_cond);
        p->has_worker_cond = false;
    }
    if (p->has_worker_mutex) {
        pthread_mutex_destroy(&p->worker_mutex);
        p->has_worker_mutex = false;
    }
    if (p->has_reader_cond) {
        pthread_cond_destroy(&p->reader_cond);
        p->has_reader_cond = false;
    }
    if (p->has_reader_mutex) {
        pthread_mutex_destroy(&p->reader_mutex);
        p->has_reader_mutex = false;
    }
    if (p->output) {
        free(p->output);
        p->output = NULL;
    }
    if (p->input) {
        free(p->input);
        p->input = NULL;
    }
    if (p->fft) {
        spek_fft_delete(p->fft);
        p->fft = NULL;
    }
    if (p->coss) {
        free(p->coss);
        p->coss = NULL;
    }
    if (p->cx) {
        spek_audio_close(p->cx);
        p->cx = NULL;
    }
    free(p);
}

static void * reader_func(void *pp)
{
    struct spek_pipeline *p = pp;

    p->has_worker_thread = !pthread_create(&p->worker_thread, NULL, &worker_func, p);
    if (!p->has_worker_thread) {
        return NULL;
    }

    int pos = 0, prev_pos = 0;
    int block_size = p->properties->width * p->properties->channels / 8;
    int size;
    while ((size = spek_audio_read(p->cx)) > 0) {
        if (p->quit) break;

        uint8_t *buffer = p->properties->buffer;
        while (size >= block_size) {
            p->input[pos] = average_input(p, buffer);
            buffer += block_size;
            size -= block_size;
            pos = (pos + 1) % p->input_size;

            // Wake up the worker if we have enough data.
            if ((pos > prev_pos ? pos : pos + p->input_size) - prev_pos == p->nfft * NFFT) {
                reader_sync(p, prev_pos = pos);
            }
        }
        assert(size == 0);
    }

    if (pos != prev_pos) {
        // Process the remaining data.
        reader_sync(p, pos);
    }

    // Force the worker to quit.
    reader_sync(p, -1);
    pthread_join(p->worker_thread, NULL);

    // Notify the client.
    p->cb(-1, NULL, p->cb_data);
    return NULL;
}

static void reader_sync(struct spek_pipeline *p, int pos)
{
    pthread_mutex_lock(&p->reader_mutex);
    while (!p->worker_done) {
        pthread_cond_wait(&p->reader_cond, &p->reader_mutex);
    }
    p->worker_done = false;
    pthread_mutex_unlock(&p->reader_mutex);

    pthread_mutex_lock(&p->worker_mutex);
    p->input_pos = pos;
    pthread_cond_signal(&p->worker_cond);
    pthread_mutex_unlock(&p->worker_mutex);
}

static void * worker_func(void *pp)
{
    struct spek_pipeline *p = pp;

    int sample = 0;
    int64_t frames = 0;
    int64_t num_fft = 0;
    int64_t acc_error = 0;
    int head = 0, tail = 0;
    int prev_head = 0;

    memset(p->output, 0, sizeof(float) * p->bands);

    while (true) {
        pthread_mutex_lock(&p->reader_mutex);
        p->worker_done = true;
        pthread_cond_signal(&p->reader_cond);
        pthread_mutex_unlock(&p->reader_mutex);

        pthread_mutex_lock(&p->worker_mutex);
        while (tail == p->input_pos) {
            pthread_cond_wait(&p->worker_cond, &p->worker_mutex);
        }
        tail = p->input_pos;
        pthread_mutex_unlock(&p->worker_mutex);

        if (tail == -1) {
            return NULL;
        }

        while (true) {
            head = (head + 1) % p->input_size;
            if (head == tail) {
                head = prev_head;
                break;
            }
            frames++;

            // If we have enough frames for an FFT or we have
            // all frames required for the interval run and FFT.
            bool int_full =
                acc_error < p->properties->error_base &&
                frames == p->properties->frames_per_interval;
            bool int_over =
                acc_error >= p->properties->error_base &&
                frames == 1 + p->properties->frames_per_interval;

            if (frames % p->nfft == 0 || ((int_full || int_over) && num_fft == 0)) {
                prev_head = head;
                for (int i = 0; i < p->nfft; i++) {
                    float val = p->input[(p->input_size + head - p->nfft + i) % p->input_size];
                    // TODO: allow the user to chose the window function
                    // Hamming window.
                    // val *= 0.53836f - 0.46164f * coss[i];
                    // Hann window.
                    val *= 0.5f * (1.0f - p->coss[i]);
                    p->fft->input[i] = val;
                }
                spek_fft_execute(p->fft);
                num_fft++;
                for (int i = 0; i < p->bands; i++) {
                    p->output[i] += p->fft->output[i];
                }
            }

            // Do we have the FFTs for one interval?
            if (int_full || int_over) {
                if (int_over) {
                    acc_error -= p->properties->error_base;
                } else {
                    acc_error += p->properties->error_per_interval;
                }

                for (int i = 0; i < p->bands; i++) {
                    p->output[i] /= num_fft;
                }

                if (sample == p->samples) break;
                p->cb(sample++, p->output, p->cb_data);

                memset(p->output, 0, sizeof(float) * p->bands);
                frames = 0;
                num_fft = 0;
            }
        }
    }
}

static float average_input(const struct spek_pipeline *p, void *buffer)
{
    int channels = p->properties->channels;
    float res = 0.0f;
    if (p->properties->fp) {
        if (p->properties->width == 32) {
            float *b = buffer;
            for (int i = 0; i < channels; i++) {
                res += b[i];
            }
        } else {
            assert(p->properties->width == 64);
            double *b = buffer;
            for (int i = 0; i < channels; i++) {
                res += (float) b[i];
            }
        }
    } else {
        if (p->properties->width == 16) {
            int16_t *b = buffer;
            for (int i = 0; i < channels; i++) {
                res += b[i] / (float) INT16_MAX;
            }
        } else {
            assert (p->properties->width == 32);
            int32_t *b = buffer;
            for (int i = 0; i < channels; i++) {
                res += b[i] / (float) INT32_MAX;
            }
        }
    }
    return res / channels;
}
