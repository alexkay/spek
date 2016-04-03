#pragma once

#include <memory>
#include <vector>

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
        input(input_size), output(output_size) {}
    virtual ~FFTPlan() {}

    int get_input_size() const { return this->input_size; }
    int get_output_size() const { return this->output_size; }
    float get_input(int i) const { return this->input[i]; }
    void set_input(int i, float v) { this->input[i] = v; }
    float get_output(int i) const { return this->output[i]; }
    void set_output(int i, float v) { this->output[i] = v; }

    virtual void execute() = 0;

protected:
    float *get_input() { return this->input.data(); }

private:
    int input_size;
    int output_size;
    std::vector<float> input;
    std::vector<float> output;
};
