/* test-audio-read.cc
 *
 * Copyright (C) 2013  Alexander Kojevnikov <alexander@kojevnikov.com>
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

#include <map>

#include "spek-audio.h"

#include "test.h"

static void test_silence(const std::string& name, int samples)
{
    Audio audio;
    auto file = audio.open(SAMPLES_DIR "/" + name);

    int samples_read = 0;
    double power = 0.0;
    int len;
    while ((len = file->read()) > 0) {
        samples_read += len;
        for (int i = 0; i < len; ++i) {
            float level = file->get_buffer()[i];
            power += level * level;
        }
    }
    power /= samples_read;

    test("error", 0, len);
    test("samples", samples, samples_read);
    test("power", 0.0, power);
}

void test_audio_read()
{
    std::map<std::string, int> files = {
        {"1ch-96000Hz-24bps.flac", 1 * 96000 / 10},
        {"2ch-48000Hz-16bps.flac", 2 * 48000 / 10},
        {"1ch-96000Hz-24bps.ape", 1 * 96000 / 10},
        {"2ch-48000Hz-16bps.ape", 2 * 48000 / 10},
        {"2ch-44100Hz-16bps.m4a", 2 * 44100 / 10},
        {"1ch-96000Hz-24bps.wv", 1 * 96000 / 10},
        {"2ch-48000Hz-16bps.wv", 2 * 48000 / 10},
        {"2ch-44100Hz-16bps.wav", 2 * 44100 / 10},
        {"2ch-44100Hz-128cbr.mp3", 2 * 1152 * 4 + 94},
        {"2ch-44100Hz-320cbr.mp3", 2 * 1152 * 4 + 94},
        {"2ch-44100Hz-V0.mp3", 2 * 1152 * 4 + 94},
        {"2ch-44100Hz-V2.mp3", 2 * 1152 * 4 + 94},
        {"2ch-44100Hz-q100.m4a", 10240},
        {"2ch-44100Hz-q5.ogg", 2 * 1024 * 4 + 1152},
        {"2ch-44100Hz.dts", 10240},
        {"2ch-44100Hz.ac3", 9 * 1024},
        {"2ch-44100Hz-std.mpc", 11 * 1024 + 256},
    };
    for (const auto& item : files) {
        run(
            "audio read: " + item.first,
            [&] () { test_silence(item.first, item.second); }
        );
    }
}
