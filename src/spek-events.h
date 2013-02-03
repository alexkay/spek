/* spek-events.h
 *
 * Copyright (C) 2012  Alexander Kojevnikov <alexander@kojevnikov.com>
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

#ifndef SPEK_EVENTS_H_
#define SPEK_EVENTS_H_

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
//    DECLARE_DYNAMIC_CLASS(SpekHaveSampleEvent);

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

#endif
