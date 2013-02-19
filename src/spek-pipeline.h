/* spek-pipeline.h
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

#ifndef SPEK_PIPELINE_H_
#define SPEK_PIPELINE_H_

#include <memory>
#include <string>

class Audio;
struct spek_pipeline;

typedef void (*spek_pipeline_cb)(int sample, float *values, void *cb_data);

struct spek_pipeline * spek_pipeline_open(
    std::unique_ptr<AudioFile> file, int bands, int samples, spek_pipeline_cb cb, void *cb_data
);

void spek_pipeline_start(struct spek_pipeline *pipeline);
void spek_pipeline_close(struct spek_pipeline *pipeline);

std::string spek_pipeline_desc(const struct spek_pipeline *pipeline);
double spek_pipeline_duration(const struct spek_pipeline *pipeline);
int spek_pipeline_sample_rate(const struct spek_pipeline *pipeline);

#endif
