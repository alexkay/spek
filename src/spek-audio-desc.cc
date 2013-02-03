/* spek-audio-desc.cc
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

#include <wx/arrstr.h>
#include <wx/intl.h>

#include <spek-audio.h>

#include "spek-audio-desc.h"

#define ngettext wxPLURAL

wxString spek_audio_desc(const struct spek_audio_properties *properties)
{
    wxArrayString items;

    if (properties->codec_name) {
        items.Add(wxString::FromUTF8(properties->codec_name));
    }
    if (properties->bit_rate) {
        items.Add(wxString::Format(_("%d kbps"), (properties->bit_rate + 500) / 1000));
    }
    if (properties->sample_rate) {
        items.Add(wxString::Format(_("%d Hz"), properties->sample_rate));
    }
    // Include bits per sample only if there is no bitrate.
    if (properties->bits_per_sample && !properties->bit_rate) {
        items.Add(wxString::Format(
            ngettext("%d bit", "%d bits", properties->bits_per_sample),
            properties->bits_per_sample
        ));
    }
    if (properties->channels) {
        items.Add(wxString::Format(
            ngettext("%d channel", "%d channels", properties->channels),
            properties->channels
        ));
    }

    wxString desc;
    for (int i = 0; i < items.GetCount(); ++i) {
        if (i) {
            desc += wxT(", ");
        }
        desc += items[i];
    }

    if (properties->error) {
        wxString error;
        switch (properties->error) {
        case SPEK_AUDIO_CANNOT_OPEN_FILE:
            error = _("Cannot open input file");
            break;
        case SPEK_AUDIO_NO_STREAMS:
            error = _("Cannot find stream info");
            break;
        case SPEK_AUDIO_NO_AUDIO:
            error = _("The file contains no audio streams");
            break;
        case SPEK_AUDIO_NO_DECODER:
            error = _("Cannot find decoder");
            break;
        case SPEK_AUDIO_NO_DURATION:
            error = _("Unknown duration");
            break;
        case SPEK_AUDIO_NO_CHANNELS:
            error = _("No audio channels");
            break;
        case SPEK_AUDIO_CANNOT_OPEN_DECODER:
            error = _("Cannot open decoder");
            break;
        case SPEK_AUDIO_BAD_SAMPLE_FORMAT:
            error = _("Unsupported sample format");
            break;
        }

        if (desc.IsEmpty()) {
            desc = error;
        } else {
            // TRANSLATORS: first %s is the error message, second %s is stream description.
            desc = wxString::Format(_("%s: %s"), error.c_str(), desc.c_str());
        }
    }

    return desc;
}
