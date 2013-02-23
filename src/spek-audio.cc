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

extern "C" {
#define __STDC_CONSTANT_MACROS
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/mathematics.h>
}

#include "spek-audio.h"

class AudioFileImpl : public AudioFile
{
public:
    AudioFileImpl(
        AudioError error, AVFormatContext *format_context, int audio_stream,
        const std::string& codec_name, int bit_rate, int sample_rate, int bits_per_sample,
        int channels, double duration, bool is_planar, int width, bool fp
    );
    ~AudioFileImpl() override;
    void start(int samples) override;
    int read() override;

    AudioError get_error() const override { return this->error; }
    std::string get_codec_name() const override { return this->codec_name; }
    int get_bit_rate() const override { return this->bit_rate; }
    int get_sample_rate() const override { return this->sample_rate; }
    int get_bits_per_sample() const override { return this->bits_per_sample; }
    int get_channels() const override { return this->channels; }
    double get_duration() const override { return this->duration; }
    int get_width() const override { return this->width; }
    bool get_fp() const override { return this->fp; }
    const uint8_t *get_buffer() const override { return this->buffer; }
    int64_t get_frames_per_interval() const override { return this->frames_per_interval; }
    int64_t get_error_per_interval() const override { return this->error_per_interval; }
    int64_t get_error_base() const override { return this->error_base; }

private:
    AudioError error;
    AVFormatContext *format_context;
    int audio_stream;
    std::string codec_name;
    int bit_rate;
    int sample_rate;
    int bits_per_sample;
    int channels;
    double duration;
    bool is_planar;
    int width;
    bool fp;

    AVPacket packet;
    int offset;
    AVFrame *frame;
    int buffer_size;
    uint8_t *buffer;
    // TODO: these guys don't belong here, move them somewhere else when revamping the pipeline
    int64_t frames_per_interval;
    int64_t error_per_interval;
    int64_t error_base;
};


Audio::Audio()
{
    av_register_all();
}

std::unique_ptr<AudioFile> Audio::open(const std::string& file_name)
{
    AudioError error = AudioError::OK;

    AVFormatContext *format_context = nullptr;
    if (avformat_open_input(&format_context, file_name.c_str(), nullptr, nullptr) != 0) {
        error = AudioError::CANNOT_OPEN_FILE;
    }

    if (!error && avformat_find_stream_info(format_context, nullptr) < 0) {
        // 24-bit APE returns an error but parses the stream info just fine.
        // TODO: old comment, verify
        if (format_context->nb_streams <= 0) {
            error = AudioError::NO_STREAMS;
        }
    }

    int audio_stream = -1;
    if (!error) {
        for (unsigned int i = 0; i < format_context->nb_streams; i++) {
            if (format_context->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
                audio_stream = i;
                break;
            }
        }
    }
    if (audio_stream == -1) {
        error = AudioError::NO_AUDIO;
    }

    AVStream *stream = nullptr;
    AVCodecContext *codec_context = nullptr;
    AVCodec *codec = nullptr;
    if (!error) {
        stream = format_context->streams[audio_stream];
        codec_context = stream->codec;
        codec = avcodec_find_decoder(codec_context->codec_id);
        if (!codec) {
            error = AudioError::NO_DECODER;
        }
    }

    std::string codec_name;
    int bit_rate = 0;
    int sample_rate = 0;
    int bits_per_sample = 0;
    int channels = 0;
    double duration = 0;
    if (!error) {
        // We can already fill in the stream info even if the codec won't be able to open it.
        codec_name = codec->long_name;
        bit_rate = codec_context->bit_rate;
        sample_rate = codec_context->sample_rate;
        bits_per_sample = codec_context->bits_per_raw_sample;
        if (!bits_per_sample) {
            // APE uses bpcs, FLAC uses bprs.
            // TODO: old comment, verify
            bits_per_sample = codec_context->bits_per_coded_sample;
        }
        channels = codec_context->channels;

        if (stream->duration != AV_NOPTS_VALUE) {
            duration = stream->duration * av_q2d(stream->time_base);
        } else if (format_context->duration != AV_NOPTS_VALUE) {
            duration = format_context->duration / (double) AV_TIME_BASE;
        } else {
            error = AudioError::NO_DURATION;
        }

        if (!error && channels <= 0) {
            error = AudioError::NO_CHANNELS;
        }
    }

    if (!error && avcodec_open2(codec_context, codec, nullptr) < 0) {
        error = AudioError::CANNOT_OPEN_DECODER;
    }

    bool is_planar = false;
    int width = 0;
    bool fp = false;
    if (!error) {
        is_planar = av_sample_fmt_is_planar(codec_context->sample_fmt);
        width = av_get_bytes_per_sample(codec_context->sample_fmt);
        AVSampleFormat fmt = codec_context->sample_fmt;
        if (fmt == AV_SAMPLE_FMT_S16 || fmt == AV_SAMPLE_FMT_S16P ||
            fmt == AV_SAMPLE_FMT_S32 || fmt == AV_SAMPLE_FMT_S32P) {
            fp = false;
        } else if (fmt == AV_SAMPLE_FMT_FLT || fmt == AV_SAMPLE_FMT_FLTP ||
            fmt == AV_SAMPLE_FMT_DBL || fmt == AV_SAMPLE_FMT_DBLP ) {
            fp = true;
        } else {
            error = AudioError::BAD_SAMPLE_FORMAT;
        }
    }

    return std::unique_ptr<AudioFile>(new AudioFileImpl(
        error, format_context, audio_stream,
        codec_name, bit_rate, sample_rate, bits_per_sample,
        channels, duration, is_planar, width, fp
    ));
}

AudioFileImpl::AudioFileImpl(
    AudioError error, AVFormatContext *format_context, int audio_stream,
    const std::string& codec_name, int bit_rate, int sample_rate, int bits_per_sample,
    int channels, double duration, bool is_planar, int width, bool fp
) :
    error(error), format_context(format_context), audio_stream(audio_stream),
    codec_name(codec_name), bit_rate(bit_rate),
    sample_rate(sample_rate), bits_per_sample(bits_per_sample),
    channels(channels), duration(duration), is_planar(is_planar), width(width), fp(fp)
{
    av_init_packet(&this->packet);
    this->packet.data = nullptr;
    this->packet.size = 0;
    this->offset = 0;
    this->frame = avcodec_alloc_frame();
    this->buffer_size = 0;
    this->buffer = nullptr;
    this->frames_per_interval = 0;
    this->error_per_interval = 0;
    this->error_base = 0;
}

AudioFileImpl::~AudioFileImpl()
{
    if (this->buffer) {
        av_freep(&this->buffer);
    }
    if (this->frame) {
        // TODO: Remove this check after Debian switches to libav 9.
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(54, 28, 0)
        avcodec_free_frame(&this->frame);
#else
        av_freep(&this->frame);
#endif
    }
    if (this->packet.data) {
        this->packet.data -= this->offset;
        this->packet.size += this->offset;
        this->offset = 0;
        av_free_packet(&this->packet);
    }
    if (this->format_context) {
        if (this->audio_stream >= 0) {
            auto codec_context = this->format_context->streams[this->audio_stream]->codec;
            if (codec_context) {
                avcodec_close(codec_context);
            }
        }
        avformat_close_input(&this->format_context);
    }
}

void AudioFileImpl::start(int samples)
{
    AVStream *stream = this->format_context->streams[this->audio_stream];
    int64_t rate = this->sample_rate * (int64_t)stream->time_base.num;
    int64_t duration = (int64_t)(this->duration * stream->time_base.den / stream->time_base.num);
    this->error_base = samples * (int64_t)stream->time_base.den;
    this->frames_per_interval = av_rescale_rnd(duration, rate, this->error_base, AV_ROUND_DOWN);
    this->error_per_interval = (duration * rate) % this->error_base;
}

int AudioFileImpl::read()
{
    if (!!this->error) {
        return -1;
    }

    for (;;) {
        while (this->packet.size > 0) {
            avcodec_get_frame_defaults(this->frame);
            auto codec_context = this->format_context->streams[this->audio_stream]->codec;
            int got_frame = 0;
            int len = avcodec_decode_audio4(codec_context, this->frame, &got_frame, &this->packet);
            if (len < 0) {
                // Error, skip the frame.
                break;
            }
            this->packet.data += len;
            this->packet.size -= len;
            this->offset += len;
            if (!got_frame) {
                // No data yet, get more frames.
                continue;
            }
            // We have data, return it and come back for more later.
            int samples = this->frame->nb_samples;
            int channels = this->channels;
            int width = this->width;
            int buffer_size = samples * channels * width;
            if (buffer_size > this->buffer_size) {
                this->buffer = (uint8_t*)av_realloc(this->buffer, buffer_size);
                this->buffer_size = buffer_size;
            }
            if (this->is_planar) {
                for (int channel = 0; channel < channels; ++channel) {
                    uint8_t *buffer = this->buffer + channel * width;
                    uint8_t *data = this->frame->data[channel];
                    for (int sample = 0; sample < samples; ++sample) {
                        for (int i = 0; i < width; ++i) {
                            *buffer++ = *data++;
                        }
                        buffer += (channels - 1) * width;
                    }
                }
            } else {
                memcpy(this->buffer, this->frame->data[0], buffer_size);
            }
            return buffer_size;
        }
        if (this->packet.data) {
            this->packet.data -= this->offset;
            this->packet.size += this->offset;
            this->offset = 0;
            av_free_packet(&this->packet);
        }

        int res = 0;
        while ((res = av_read_frame(this->format_context, &this->packet)) >= 0) {
            if (this->packet.stream_index == this->audio_stream) {
                break;
            }
            av_free_packet(&this->packet);
        }
        if (res < 0) {
            // End of file or error.
            return 0;
        }
    }
}
