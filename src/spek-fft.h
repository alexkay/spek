#pragma once

#include <memory>
#include <vector>

extern "C" {
#include <libavutil/mem.h>
}


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
        this->input = (float*) av_malloc(sizeof(float) * input_size);
    }

    virtual ~FFTPlan()
    {
        av_freep(this->input);
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
