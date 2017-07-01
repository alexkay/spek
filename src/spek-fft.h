#pragma once

#include <memory>
#include <vector>

#include "spek-platform.h"

class FFTPlan;

class FFT
{
public:
    FFT() {}
    std::unique_ptr<FFTPlan> create(int nbits);
};

class FFTPlan
{
public:
    FFTPlan(int nbits) :
        input_size(1 << nbits), output_size((1 << (nbits - 1)) + 1),
        output(output_size)
    {
        // FFmpeg uses various assembly optimizations which expect
        // input data to be aligned by up to 32 bytes (e.g. AVX)
        this->input = (float*) spek_platform_aligned_alloc(32, sizeof(float) * input_size);
    }
    virtual ~FFTPlan()
    {
        spek_platform_aligned_free(this->input);
    }

    int get_input_size() const { return this->input_size; }
    int get_output_size() const { return this->output_size; }
    float get_input(int i) const { return this->input[i]; }
    void set_input(int i, float v) { this->input[i] = v; }
    float get_output(int i) const { return this->output[i]; }
    void set_output(int i, float v) { this->output[i] = v; }

    virtual void execute() = 0;

protected:
    float *get_input() { return this->input; }

private:
    int input_size;
    int output_size;
    float *input;
    std::vector<float> output;
};
