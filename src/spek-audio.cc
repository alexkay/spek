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
#define __STDC_LIMIT_MACROS
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/mathematics.h>
}

#include "spek-audio.h"

// TODO: Remove these macros after Debian switches to libav 9.
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(54, 28, 0)
#define avcodec_free_frame av_freep
#endif
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(54, 25, 0)
#define AV_CODEC_ID_AAC CODEC_ID_AAC
#define AV_CODEC_ID_MUSEPACK8 CODEC_ID_MUSEPACK8
#define AV_CODEC_ID_WMAV1 CODEC_ID_WMAV1
#define AV_CODEC_ID_WMAV2 CODEC_ID_WMAV2
#endif

class AudioFileImpl : public AudioFile
{
public:
    AudioFileImpl(
        AudioError error, AVFormatContext *format_context, int audio_stream,
        const std::string& codec_name, int bit_rate, int sample_rate, int bits_per_sample,
        int channels, double duration
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
    const float *get_buffer() const override { return this->buffer; }
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

    AVPacket packet;
    int offset;
    AVFrame *frame;
    int buffer_len;
    float *buffer;
    // TODO: these guys don't belong here, move them somewhere else when revamping the pipeline
    int64_t frames_per_interval;
    int64_t error_per_interval;
    int64_t error_base;
};


Audio::Audio()
{
    av_register_all();
}

Audio::~Audio()
{
    // This prevents a memory leak.
    av_lockmgr_register(nullptr);
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
        if (audio_stream == -1) {
            error = AudioError::NO_AUDIO;
        }
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
            bits_per_sample = codec_context->bits_per_coded_sample;
        }
        if (codec_context->codec_id == AV_CODEC_ID_AAC ||
            codec_context->codec_id == AV_CODEC_ID_MUSEPACK8 ||
            codec_context->codec_id == AV_CODEC_ID_WMAV1 ||
            codec_context->codec_id == AV_CODEC_ID_WMAV2) {
            // These decoders set both bps and bitrate.
            bits_per_sample = 0;
        }
        if (bits_per_sample) {
            bit_rate = 0;
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

    if (!error) {
        AVSampleFormat fmt = codec_context->sample_fmt;
        if (fmt != AV_SAMPLE_FMT_S16 && fmt != AV_SAMPLE_FMT_S16P &&
            fmt != AV_SAMPLE_FMT_S32 && fmt != AV_SAMPLE_FMT_S32P &&
            fmt != AV_SAMPLE_FMT_FLT && fmt != AV_SAMPLE_FMT_FLTP &&
            fmt != AV_SAMPLE_FMT_DBL && fmt != AV_SAMPLE_FMT_DBLP ) {
            error = AudioError::BAD_SAMPLE_FORMAT;
        }
    }

    return std::unique_ptr<AudioFile>(new AudioFileImpl(
        error, format_context, audio_stream,
        codec_name, bit_rate, sample_rate, bits_per_sample,
        channels, duration
    ));
}

AudioFileImpl::AudioFileImpl(
    AudioError error, AVFormatContext *format_context, int audio_stream,
    const std::string& codec_name, int bit_rate, int sample_rate, int bits_per_sample,
    int channels, double duration
) :
    error(error), format_context(format_context), audio_stream(audio_stream),
    codec_name(codec_name), bit_rate(bit_rate),
    sample_rate(sample_rate), bits_per_sample(bits_per_sample),
    channels(channels), duration(duration)
{
    av_init_packet(&this->packet);
    this->packet.data = nullptr;
    this->packet.size = 0;
    this->offset = 0;
    this->frame = av_frame_alloc();
    this->buffer_len = 0;
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
        av_frame_free(&this->frame);
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
            av_frame_unref(this->frame);
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
            int buffer_len = samples * channels;
            if (buffer_len > this->buffer_len) {
                this->buffer = static_cast<float*>(
                    av_realloc(this->buffer, buffer_len * sizeof(float))
                );
                this->buffer_len = buffer_len;
            }

            AVSampleFormat format = static_cast<AVSampleFormat>(this->frame->format);
            int is_planar = av_sample_fmt_is_planar(format);
            int i = 0;
            for (int sample = 0; sample < samples; ++sample) {
                for (int channel = 0; channel < channels; ++channel) {
                    uint8_t *data;
                    int offset;
                    if (is_planar) {
                        data = this->frame->data[channel];
                        offset = sample;
                    } else {
                        data = this->frame->data[0];
                        offset = i;
                    }
                    float value;
                    switch (format) {
                    case AV_SAMPLE_FMT_S16:
                    case AV_SAMPLE_FMT_S16P:
                        value = reinterpret_cast<int16_t*>(data)[offset]
                            / static_cast<float>(INT16_MAX);
                        break;
                    case AV_SAMPLE_FMT_S32:
                    case AV_SAMPLE_FMT_S32P:
                        value = reinterpret_cast<int32_t*>(data)[offset]
                            / static_cast<float>(INT32_MAX);
                        break;
                    case AV_SAMPLE_FMT_FLT:
                    case AV_SAMPLE_FMT_FLTP:
                        value = reinterpret_cast<float*>(data)[offset];
                        break;
                    case AV_SAMPLE_FMT_DBL:
                    case AV_SAMPLE_FMT_DBLP:
                        value = reinterpret_cast<double*>(data)[offset];
                        break;
                    default:
                        value = 0.0f;
                        break;
                    }
                    this->buffer[i++] = value;
                }
            }
            return buffer_len;
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
