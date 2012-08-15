/* spek-pipeline.h
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

#ifndef SPEK_PIPELINE_H_
#define SPEK_PIPELINE_H_

#ifdef __cplusplus
extern "C" {
#endif

struct spek_pipeline;
struct spek_audio_properties;

typedef void (*spek_pipeline_cb)(int sample, float *values, void *cb_data);

struct spek_pipeline * spek_pipeline_open(
    const char *path, int bands, int samples, int threshold, spek_pipeline_cb cb, void *cb_data);

const struct spek_audio_properties * spek_pipeline_properties(struct spek_pipeline *pipeline);

void spek_pipeline_start(struct spek_pipeline *pipeline);

void spek_pipeline_close(struct spek_pipeline *pipeline);

#ifdef __cplusplus
}
#endif

#endif
