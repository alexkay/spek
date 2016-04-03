#include "spek-events.h"

//IMPLEMENT_DYNAMIC_CLASS(SpekHaveSampleEvent, wxEvent)
DEFINE_EVENT_TYPE(SPEK_HAVE_SAMPLE)

SpekHaveSampleEvent::SpekHaveSampleEvent(int bands, int sample, float *values, bool free_values)
    : wxEvent(), bands(bands), sample(sample), values(values), free_values(free_values)
{
    SetEventType(SPEK_HAVE_SAMPLE);
}

SpekHaveSampleEvent::SpekHaveSampleEvent(const SpekHaveSampleEvent& other) : wxEvent(other)
{
    SetEventType(SPEK_HAVE_SAMPLE);
    this->bands = other.bands;
    this->sample = other.sample;
    if (other.values) {
        this->values = (float *)malloc(this->bands * sizeof(float));
        memcpy(this->values, other.values, this->bands * sizeof(float));
        this->free_values = true;
    } else {
        this->values = NULL;
        this->free_values = false;
    }
}

SpekHaveSampleEvent::~SpekHaveSampleEvent()
{
    if (this->free_values) {
        free(this->values);
    }
}
