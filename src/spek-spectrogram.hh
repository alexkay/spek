/* spek-spectrogram.hh
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

#ifndef SPEK_SPECTROGRAM_HH_
#define SPEK_SPECTROGRAM_HH_

#include <wx/wx.h>

class SpekHaveSampleEvent;
struct spek_audio_properties;
struct spek_pipeline;

class SpekSpectrogram : public wxWindow
{
public:
    SpekSpectrogram(wxFrame *parent);
    ~SpekSpectrogram();
    void open(const wxString& path);
    void save(const wxString& path);

private:
    void on_char(wxKeyEvent& evt);
    void on_idle(wxIdleEvent& evt);
    void on_paint(wxPaintEvent& evt);
    void on_size(wxSizeEvent& evt);
    void on_have_sample(SpekHaveSampleEvent& evt);
    void render(wxDC& dc);

    void start();
    void stop();

    spek_pipeline *pipeline;
    wxString path;
    wxString desc;
    double duration;
    int sample_rate;
    wxImage palette;
    wxImage image;
    int prev_width;

    DECLARE_EVENT_TABLE()
};

#endif
