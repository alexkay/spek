/* spek-fft.h
 *
 * Copyright (C) 2010  Alexander Kojevnikov <alexander@kojevnikov.com>
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

#ifndef __SPEK_FFT_H__
#define __SPEK_FFT_H__

#include <glib.h>
#include <fftw3.h>

typedef struct {
	/* Internal data */
	fftwf_plan plan;
	gint n;
	gint threshold;
	fftwf_complex *result;

	/* Exposed properties */
	gfloat *input;
	gfloat *output;
} SpekFftPlan;

/* Allocate buffers and create a new FFT plan */
SpekFftPlan * spek_fft_plan_new (gint n, gint threshold);

/* Execute the FFT on plan->input */
void spek_fft_execute (SpekFftPlan *p);

/* Destroy the plan and de-allocate buffers */
void spek_fft_destroy (SpekFftPlan *p);

#endif
