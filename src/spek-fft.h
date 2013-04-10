/* spek-fft.h
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

#ifndef SPEK_FFT_H_
#define SPEK_FFT_H_

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
        nbits(nbits), input_size(1 << nbits), output_size((1 << (nbits - 1)) + 1),
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
    int nbits;
    int input_size;
    int output_size;
    std::vector<float> input;
    std::vector<float> output;
};

#endif
