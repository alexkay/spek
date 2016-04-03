#include <cstdlib>
#include <iostream>
#include <fstream>

const char *SAMPLE_FILE = SAMPLES_DIR "/perf.wav";
const int SAMPLE_RATE = 44100;
const int SAMPLE_DURATION = 8 * 60; // 8 minutes
const int SAMPLES = SAMPLE_RATE * SAMPLE_DURATION; // per channel

// Some of the tests will use a sample .wav file which is auto-generated if it doesn't exists.
static void create_samples()
{
    if (std::ifstream(SAMPLE_FILE).good()) {
        return;
    }

    std::ofstream file(SAMPLE_FILE, std::ofstream::binary);
    const int data_size = SAMPLES * 2 * 2; // samples * channels * bytes per sample
    uint16_t u16;
    uint32_t u32;
    file.write("RIFF", 4);
    file.write(reinterpret_cast<const char*>(&(u32 = 36 + data_size)), 4); // size
    file.write("WAVE", 4);
    file.write("fmt ", 4);
    file.write(reinterpret_cast<const char*>(&(u32 = 16)), 4); // subchunk size
    file.write(reinterpret_cast<const char*>(&(u16 = 1)), 2); // 1 = PCM
    file.write(reinterpret_cast<const char*>(&(u16 = 2)), 2); // 2 = stereo
    file.write(reinterpret_cast<const char*>(&(u32 = SAMPLE_RATE)), 4);
    file.write(reinterpret_cast<const char*>(&(u32 = SAMPLE_RATE * 2 * 2)), 4); // byte rate
    file.write(reinterpret_cast<const char*>(&(u16 = 2 * 2)), 2); // block align
    file.write(reinterpret_cast<const char*>(&(u16 = 16)), 2); // bits per sample
    file.write("data", 4);
    file.write(reinterpret_cast<const char*>(&(u32 = data_size)), 4);

    // Fill in both channels with pseudo-random values.
    srand(93);
    for (int i = 0; i < SAMPLES * 2; ++i) {
        file.write(reinterpret_cast<const char*>(&(u16 = rand())), 2);
    }
}

// Reading and decoding an audio file.
static void perf_decoder()
{
}

// Running FFTs and processing the results.
static void perf_worker()
{
}

// Managing worker and decoder threads (in isolation from the actual decoder and worker).
static void perf_pipeline()
{
}

// Testing it all together.
static void perf_all()
{
}

// Performance regression tests.
int main()
{
    create_samples();

    perf_decoder();
    perf_worker();
    perf_pipeline();
    perf_all();

    return 0;
}
