/* spek-fft.c
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

#include <math.h>

#include "spek-fft.h"

SpekFftPlan * spek_fft_plan_new (gint n, gint threshold) {
	SpekFftPlan *p = g_new0 (SpekFftPlan, 1);
	p->input = (gfloat *) fftwf_malloc (sizeof (gfloat) * n);
	p->output = (gfloat *) fftwf_malloc (sizeof (gfloat) * (n / 2 + 1));
	p->result = (fftwf_complex *) fftwf_malloc (sizeof (fftwf_complex) * (n / 2 + 1));
	p->n = n;
	p->threshold = threshold;
	p->plan = fftwf_plan_dft_r2c_1d (n, p->input, p->result, FFTW_ESTIMATE);
	return p;
}

void spek_fft_execute (SpekFftPlan *p) {
	int i;
	int bands = p->n / 2 + 1;

	fftwf_execute (p->plan);

	/* Calculate magnitudes */
	for (i = 0; i < bands; i++) {
		gfloat val;
		val = p->result[i][0] * p->result[i][0] + p->result[i][1] * p->result[i][1];
		val /= p->n * p->n;
		val = 10.0 * log10f (val);
		p->output[i] = val < p->threshold ? p->threshold : val;
	}
}

void spek_fft_destroy (SpekFftPlan *p) {
	fftwf_destroy_plan (p->plan);
	fftwf_free (p->result);
	fftwf_free (p->output);
	fftwf_free (p->input);
	g_free (p);
}
