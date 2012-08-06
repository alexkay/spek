/* spek-fft.h
 *
 * Copyright (C) 2010-2012  Alexander Kojevnikov <alexander@kojevnikov.com>
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

#ifndef SPEK_FFT_H__
#define SPEK_FFT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avfft.h>

struct spek_fft_plan {
    // Internal data.
    RDFTContext *cx;
    int n;
    int threshold;

    // Exposed properties.
    float *input;
    float *output;
};

// Allocate buffers and create a new FFT plan.
struct spek_fft_plan * spek_fft_plan_new(int n, int threshold);

// Execute the FFT on plan->input.
void spek_fft_execute(struct spek_fft_plan *p);

// Destroy the plan and de-allocate buffers.
void spek_fft_delete(struct spek_fft_plan *p);

#ifdef __cplusplus
}
#endif

#endif
