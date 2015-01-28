/* spek-fft.cc
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

#include <cmath>
#include <stdlib.h>
#define __STDC_CONSTANT_MACROS
extern "C" {
#include <libavcodec/avfft.h>
}

#include "spek-fft.h"

class FFTPlanImpl : public FFTPlan
{
public:
    FFTPlanImpl(int nbits);
    ~FFTPlanImpl() override;

    void execute() override;

private:
    struct RDFTContext *cx;
};

std::unique_ptr<FFTPlan> FFT::create(int nbits)
{
    return std::unique_ptr<FFTPlan>(new FFTPlanImpl(nbits));
}

FFTPlanImpl::FFTPlanImpl(int nbits) : FFTPlan(nbits), cx(av_rdft_init(nbits, DFT_R2C))
{
}

FFTPlanImpl::~FFTPlanImpl()
{
    av_rdft_end(this->cx);
}

void FFTPlanImpl::execute()
{
    av_rdft_calc(this->cx, this->get_input());

    // Calculate magnitudes.
    int n = this->get_input_size();
    float n2 = n * n;
    this->set_output(0, 10.0f * log10f(this->get_input(0) * this->get_input(0) / n2));
    this->set_output(n / 2, 10.0f * log10f(this->get_input(1) * this->get_input(1) / n2));
    for (int i = 1; i < n / 2; i++) {
        float re = this->get_input(i * 2);
        float im = this->get_input(i * 2 + 1);
        this->set_output(i, 10.0f * log10f((re * re + im * im) / n2));
    }
}
