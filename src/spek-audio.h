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

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

struct spek_audio_context
{
    // Internal data.
    char *short_name;
    AVFormatContext *format_context;
    int audio_stream;
    AVCodecContext *codec_context;
    AVStream *stream;
    AVCodec *codec;
    int buffer_size;
    AVPacket *packet;
    int offset;

    // Exposed properties.
    char *file_name;
    char *codec_name;
    char *error;
    int bit_rate;
    int sample_rate;
    int bits_per_sample;
    int width; // number of bits used to store a sample
    bool fp; // floating-point sample representation
    int channels;
    double duration;
    uint8_t *buffer;
    int64_t frames_per_interval;
    int64_t error_per_interval;
    int64_t error_base;
};

// Initialise FFmpeg, should be called once on start up.
void spek_audio_init();

// Open the file, check if it has an audio stream which can be decoded.
// On error, initialises the `error` field in the returned context.
struct spek_audio_context * spek_audio_open(const char *file_name);

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
