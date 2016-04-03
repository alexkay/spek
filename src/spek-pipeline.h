#pragma once

#include <memory>
#include <string>

class AudioFile;
class FFTPlan;
struct spek_pipeline;

typedef void (*spek_pipeline_cb)(int bands, int sample, float *values, void *cb_data);

struct spek_pipeline * spek_pipeline_open(
    std::unique_ptr<AudioFile> file,
    std::unique_ptr<FFTPlan> fft,
    int samples,
    spek_pipeline_cb cb,
    void *cb_data
);

void spek_pipeline_start(struct spek_pipeline *pipeline);
void spek_pipeline_close(struct spek_pipeline *pipeline);

std::string spek_pipeline_desc(const struct spek_pipeline *pipeline);
double spek_pipeline_duration(const struct spek_pipeline *pipeline);
int spek_pipeline_sample_rate(const struct spek_pipeline *pipeline);
