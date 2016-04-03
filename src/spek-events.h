#pragma once

#include <wx/wx.h>

class SpekHaveSampleEvent: public wxEvent
{
public:
    SpekHaveSampleEvent(int bands, int sample, float *values, bool free_values);
    SpekHaveSampleEvent(const SpekHaveSampleEvent& other);
    ~SpekHaveSampleEvent();

    int get_bands() const { return this->bands; }
    int get_sample() const { return this->sample; }
    const float *get_values() const { return this->values; }

    wxEvent *Clone() const { return new SpekHaveSampleEvent(*this); }

private:
    int bands;
    int sample;
    float *values;
    bool free_values;
};

typedef void (wxEvtHandler::*SpekHaveSampleEventFunction)(SpekHaveSampleEvent&);

DECLARE_EVENT_TYPE(SPEK_HAVE_SAMPLE, wxID_ANY)

#define SPEK_EVT_HAVE_SAMPLE(fn) \
    DECLARE_EVENT_TABLE_ENTRY(SPEK_HAVE_SAMPLE, -1, -1, \
    (wxObjectEventFunction) (SpekHaveSampleEventFunction) &fn, (wxObject *) NULL ),
