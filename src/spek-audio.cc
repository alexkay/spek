/* spek-audio.cc
 *
 * Copyright (C) 2010-2013  Alexander Kojevnikov <alexander@kojevnikov.com>
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

#include <string.h>

#define __STDC_CONSTANT_MACROS
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/mathematics.h>
}

#include "spek-audio.h"

struct spek_audio_context
{
    AVFormatContext *format_context;
    int audio_stream;
    AVCodecContext *codec_context;
    AVStream *stream;
    AVCodec *codec;
    int buffer_size;
    AVPacket packet;
    int offset;
    int is_planar;
    AVFrame *frame;

    struct spek_audio_properties properties;
};


void spek_audio_init()
{
    av_register_all();
}

const struct spek_audio_properties * spek_audio_get_properties(struct spek_audio_context *cx)
{
    return &cx->properties;
}

struct spek_audio_context * spek_audio_open(const char *path)
{
    // TODO: malloc and initialise explicitely
    struct spek_audio_context *cx =
        (spek_audio_context *)calloc(1, sizeof(struct spek_audio_context));

    if (avformat_open_input(&cx->format_context, path, NULL, NULL) != 0) {
        cx->properties.error = SPEK_AUDIO_CANNOT_OPEN_FILE;
        return cx;
    }
    if (avformat_find_stream_info(cx->format_context, NULL) < 0) {
        // 24-bit APE returns an error but parses the stream info just fine.
        if (cx->format_context->nb_streams <= 0) {
            cx->properties.error = SPEK_AUDIO_NO_STREAMS;
            return cx;
        }
    }
    cx->audio_stream = -1;
    for (int i = 0; i < cx->format_context->nb_streams; i++) {
        if (cx->format_context->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            cx->audio_stream = i;
            break;
        }
    }
    if (cx->audio_stream == -1) {
        cx->properties.error = SPEK_AUDIO_NO_AUDIO;
        return cx;
    }
    cx->stream = cx->format_context->streams[cx->audio_stream];
    cx->codec_context = cx->stream->codec;
    cx->codec = avcodec_find_decoder(cx->codec_context->codec_id);
    if (cx->codec == NULL) {
        cx->properties.error = SPEK_AUDIO_NO_DECODER;
        return cx;
    }
    // We can already fill in the stream info even if the codec won't be able to open it.
    cx->properties.codec_name = strdup(cx->codec->long_name);
    cx->properties.bit_rate = cx->codec_context->bit_rate;
    cx->properties.sample_rate = cx->codec_context->sample_rate;
    cx->properties.bits_per_sample = cx->codec_context->bits_per_raw_sample;
    if (!cx->properties.bits_per_sample) {
        // APE uses bpcs, FLAC uses bprs.
        cx->properties.bits_per_sample = cx->codec_context->bits_per_coded_sample;
    }
    cx->properties.channels = cx->codec_context->channels;
    if (cx->stream->duration != AV_NOPTS_VALUE) {
        cx->properties.duration = cx->stream->duration * av_q2d(cx->stream->time_base);
    } else if (cx->format_context->duration != AV_NOPTS_VALUE) {
        cx->properties.duration = cx->format_context->duration / (double) AV_TIME_BASE;
    } else {
        cx->properties.error = SPEK_AUDIO_NO_DURATION;
        return cx;
    }
    if (cx->properties.channels <= 0) {
        cx->properties.error = SPEK_AUDIO_NO_CHANNELS;
        return cx;
    }
    if (avcodec_open2(cx->codec_context, cx->codec, NULL) < 0) {
        cx->properties.error = SPEK_AUDIO_CANNOT_OPEN_DECODER;
        return cx;
    }
    cx->is_planar = av_sample_fmt_is_planar(cx->codec_context->sample_fmt);
    cx->properties.width = av_get_bytes_per_sample(cx->codec_context->sample_fmt);
    switch (cx->codec_context->sample_fmt) {
    case AV_SAMPLE_FMT_S16:
    case AV_SAMPLE_FMT_S16P:
    case AV_SAMPLE_FMT_S32:
    case AV_SAMPLE_FMT_S32P:
        cx->properties.fp = false;
        break;
    case AV_SAMPLE_FMT_FLT:
    case AV_SAMPLE_FMT_FLTP:
    case AV_SAMPLE_FMT_DBL:
    case AV_SAMPLE_FMT_DBLP:
        cx->properties.fp = true;
        break;
    default:
        cx->properties.error = SPEK_AUDIO_BAD_SAMPLE_FORMAT;
        return cx;
    }
    cx->buffer_size = 0;
    cx->properties.buffer = NULL;
    av_init_packet(&cx->packet);
    cx->frame = avcodec_alloc_frame();
    cx->offset = 0;
    return cx;
}

void spek_audio_start(struct spek_audio_context *cx, int samples)
{
    int64_t rate = cx->properties.sample_rate * (int64_t) cx->stream->time_base.num;
    int64_t duration = (int64_t)
        (cx->properties.duration * cx->stream->time_base.den / cx->stream->time_base.num);
    cx->properties.error_base = samples * (int64_t)cx->stream->time_base.den;
    cx->properties.frames_per_interval = av_rescale_rnd(
        duration, rate, cx->properties.error_base, AV_ROUND_DOWN);
    cx->properties.error_per_interval = (duration * rate) % cx->properties.error_base;
}

int spek_audio_read(struct spek_audio_context *cx) {
    if (cx->properties.error) {
        return -1;
    }

    for (;;) {
        while (cx->packet.size > 0) {
            avcodec_get_frame_defaults(cx->frame);
            int got_frame = 0;
            int len = avcodec_decode_audio4(cx->codec_context, cx->frame, &got_frame, &cx->packet);
            if (len < 0) {
                // Error, skip the frame.
                break;
            }
            cx->packet.data += len;
            cx->packet.size -= len;
            cx->offset += len;
            if (!got_frame) {
                // No data yet, get more frames.
                continue;
            }
            // We have data, return it and come back for more later.
            int buffer_size =
                cx->frame->nb_samples * cx->properties.channels * cx->properties.width;
            if (buffer_size > cx->buffer_size) {
                cx->properties.buffer = (uint8_t*)av_realloc(cx->properties.buffer, buffer_size);
                cx->buffer_size = buffer_size;
            }
            memcpy(cx->properties.buffer, cx->frame->data[0], buffer_size);
            return buffer_size;
        }
        if (cx->packet.data) {
            cx->packet.data -= cx->offset;
            cx->packet.size += cx->offset;
            cx->offset = 0;
            av_free_packet (&cx->packet);
        }

        int res = 0;
        while ((res = av_read_frame(cx->format_context, &cx->packet)) >= 0) {
            if (cx->packet.stream_index == cx->audio_stream) {
                break;
            }
            av_free_packet(&cx->packet);
        }
        if (res < 0) {
            // End of file or error.
            return 0;
        }
    }
}

void spek_audio_close(struct spek_audio_context *cx)
{
    if (cx->properties.codec_name != NULL) {
        free(cx->properties.codec_name);
    }
    if (cx->frame != NULL) {
        avcodec_free_frame(&cx->frame);
    }
    if (cx->packet.data) {
        cx->packet.data -= cx->offset;
        cx->packet.size += cx->offset;
        cx->offset = 0;
        av_free_packet(&cx->packet);
    }
    if (cx->properties.buffer) {
        av_free(cx->properties.buffer);
    }
    if (cx->codec_context != NULL) {
        avcodec_close(cx->codec_context);
    }
    if (cx->format_context != NULL) {
        avformat_close_input(&cx->format_context);
    }
    free(cx);
}
