/* spek-audio.h
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

#ifndef SPEK_AUDIO_H_
#define SPEK_AUDIO_H_

#include <memory>
#include <string>

class AudioFile;
enum class AudioError;

class Audio
{
public:
    Audio();

    std::unique_ptr<AudioFile> open(const std::string& file_name);
};

class AudioFile
{
public:
    virtual ~AudioFile() {}

    virtual void start(int samples) = 0;
    virtual int read() = 0;

    virtual AudioError get_error() const = 0;
    virtual std::string get_codec_name() const = 0;
    virtual int get_bit_rate() const = 0;
    virtual int get_sample_rate() const = 0;
    virtual int get_bits_per_sample() const = 0;
    virtual int get_channels() const = 0;
    virtual double get_duration() const = 0;
    virtual int get_width() const = 0;
    virtual bool get_fp() const = 0;
    virtual const uint8_t *get_buffer() const = 0;
    virtual int64_t get_frames_per_interval() const = 0;
    virtual int64_t get_error_per_interval() const = 0;
    virtual int64_t get_error_base() const = 0;
};

enum class AudioError
{
    OK,
    CANNOT_OPEN_FILE,
    NO_STREAMS,
    NO_AUDIO,
    NO_DECODER,
    NO_DURATION,
    NO_CHANNELS,
    CANNOT_OPEN_DECODER,
    BAD_SAMPLE_FORMAT,
};

inline bool operator!(AudioError error) {
    return error == AudioError::OK;
}

#endif
