/* spek-audio.h
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
 */

#ifndef SPEK_AUDIO_H_
#define SPEK_AUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

struct spek_audio_context;

enum spek_audio_error
{
    SPEK_AUDIO_OK = 0,
    SPEK_AUDIO_CANNOT_OPEN_FILE,
    SPEK_AUDIO_NO_STREAMS,
    SPEK_AUDIO_NO_AUDIO,
    SPEK_AUDIO_NO_DECODER,
    SPEK_AUDIO_NO_DURATION,
    SPEK_AUDIO_NO_CHANNELS,
    SPEK_AUDIO_CANNOT_OPEN_DECODER,
    SPEK_AUDIO_BAD_SAMPLE_FORMAT,
};

struct spek_audio_properties
{
    char *codec_name;
    enum spek_audio_error error;
    int bit_rate;
    int sample_rate;
    int bits_per_sample;
    int width; // number of bits used to store a sample
    bool fp; // floating-point sample representation
    int channels;
    double duration;
    // TODO: these four guys don't belong here, move them somewhere else when revamping the pipeline
    uint8_t *buffer;
    int64_t frames_per_interval;
    int64_t error_per_interval;
    int64_t error_base;
};

// Initialise FFmpeg, should be called once on start up.
void spek_audio_init();

// Open the file, check if it has an audio stream which can be decoded.
// On error, initialises the `error` field in the returned context.
struct spek_audio_context * spek_audio_open(const char *path);

const struct spek_audio_properties * spek_audio_get_properties(struct spek_audio_context *cs);

// Prepare the context for reading audio samples.
void spek_audio_start(struct spek_audio_context *cx, int samples);

// Read and decode the opened audio stream.
// Returns -1 on error, 0 if there's nothing left to read
// or the number of bytes decoded into the buffer.
int spek_audio_read(struct spek_audio_context *cx);

// Closes the file opened with spek_audio_open,
// frees all allocated buffers and the context
void spek_audio_close(struct spek_audio_context *cx);

#ifdef __cplusplus
}
#endif

#endif
