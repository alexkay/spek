/* spek-events.cc
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

#include "spek-events.hh"

//IMPLEMENT_DYNAMIC_CLASS(SpekHaveSampleEvent, wxEvent)
DEFINE_EVENT_TYPE(SPEK_HAVE_SAMPLE)

SpekHaveSampleEvent::SpekHaveSampleEvent(int bands, int sample, float *values, bool free_values)
    : wxEvent(), bands(bands), sample(sample), values(values), free_values(free_values)
{
    SetEventType(SPEK_HAVE_SAMPLE);
}

SpekHaveSampleEvent::SpekHaveSampleEvent(const SpekHaveSampleEvent& other)
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
