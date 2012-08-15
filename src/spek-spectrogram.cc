/* spek-spectrogram.cc
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

#include <cmath>
#include <wx/dcbuffer.h>

#include "spek-audio-desc.hh"
#include "spek-pipeline.h"

#include "spek-spectrogram.hh"

BEGIN_EVENT_TABLE(SpekSpectrogram, wxPanel)
    EVT_PAINT(SpekSpectrogram::on_paint)
    EVT_SIZE(SpekSpectrogram::on_size)
END_EVENT_TABLE()

enum
{
    THRESHOLD = -92,
    NFFT = 2048,
    BANDS = NFFT / 2 + 1,
    LPAD = 60,
    TPAD = 60,
    RPAD = 80,
    BPAD = 40,
    GAP = 10,
    RULER = 10,
};

SpekSpectrogram::SpekSpectrogram(wxFrame *parent) :
    wxPanel(parent, -1, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE),
    pipeline(NULL),
    palette(RULER, BANDS),
    image(1, 1),
    prev_width(-1)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    // Pre-draw the palette.
    for (int y = 0; y < BANDS; y++) {
        uint32_t color = get_color(y / (double) BANDS);
        this->palette.SetRGB(
            wxRect(0, BANDS - y - 1, RULER, 1),
            color >> 16,
            (color >> 8) & 0xFF,
            color & 0xFF
        );
    }
}

void SpekSpectrogram::open(const wxString& path)
{
    this->path = path;
    start();
}

void SpekSpectrogram::save(const wxString& path)
{
}

void SpekSpectrogram::on_paint(wxPaintEvent& evt)
{
    wxAutoBufferedPaintDC dc(this);
    render(dc);
}

void SpekSpectrogram::on_size(wxSizeEvent& evt)
{
    wxSize size = GetClientSize();
    bool width_changed = this->prev_width != size.GetWidth();
    this->prev_width = size.GetWidth();

    if (!this->path.IsEmpty() && width_changed) {
        start();
    }
}

void SpekSpectrogram::render(wxDC& dc)
{
    wxSize size = GetClientSize();
    int w = size.GetWidth();
    int h = size.GetHeight();

    // Initialise.
    dc.SetBackground(*wxBLACK_BRUSH);
    dc.SetBackgroundMode(wxTRANSPARENT);
    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetTextForeground(wxColour(255, 255, 255));
    wxFont normal_font = wxFont(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    wxFont large_font = wxFont(normal_font);
    large_font.SetPointSize(10);
    large_font.SetWeight(wxFONTWEIGHT_BOLD);
    wxFont small_font = wxFont(normal_font);
    small_font.SetPointSize(8);
    dc.SetFont(normal_font);
    int normal_height = dc.GetTextExtent(wxT("dummy")).GetHeight();
    dc.SetFont(large_font);
    int large_height = dc.GetTextExtent(wxT("dummy")).GetHeight();

    // Clean the background.
    dc.Clear();

    // Spek version
    dc.SetFont(large_font);
    wxString package_name(wxT(PACKAGE_NAME));
    dc.DrawText(
        package_name,
        w - RPAD + GAP,
        TPAD - 2 * GAP - normal_height - large_height
    );
    int package_name_width = dc.GetTextExtent(package_name + wxT(" ")).GetWidth();
    dc.SetFont(normal_font);
    dc.DrawText(
        wxT(PACKAGE_VERSION),
        w - RPAD + GAP + package_name_width,
        TPAD - 2 * GAP - 2 * normal_height
    );

    if (this->image.GetHeight() > 1) {
        // Draw the spectrogram.
        wxBitmap bmp(this->image.Scale(w - LPAD - RPAD, h - TPAD - BPAD /*TODO:, wxIMAGE_QUALITY_HIGH*/));
        dc.DrawBitmap(bmp, LPAD, TPAD);
    }

    // Border around the spectrogram.
    // TODO: check if this uses antialiasing
    dc.DrawRectangle(LPAD, TPAD, w - LPAD - RPAD, h - TPAD - BPAD);

    // The palette.
    wxBitmap bmp(this->palette.Scale(RULER, h - TPAD - BPAD + 1 /*TODO:, wxIMAGE_QUALITY_HIGH*/));
    dc.DrawBitmap(bmp, w - RPAD + GAP, TPAD);
}

void SpekSpectrogram::pipeline_cb(int sample, float *values, void *cb_data)
{
    static double log10_threshold = log10(-THRESHOLD);
    SpekSpectrogram *s = (SpekSpectrogram *)cb_data;

    for (int y = 0; y < BANDS; y++) {
        double level = log10(1.0 - THRESHOLD + values[y]) / log10_threshold;
        if (level > 1.0) level = 1.0;
        uint32_t color = get_color(level);
        s->image.SetRGB(
            sample,
            BANDS - y - 1,
            color >> 16,
            (color >> 8) & 0xFF,
            color & 0xFF
        );
    }

    s->Refresh(); // TODO: refresh only one pixel column
}


void SpekSpectrogram::start()
{
    if (this->pipeline) {
        spek_pipeline_close(this->pipeline);
        this->pipeline = NULL;
    }

    // The number of samples is the number of pixels available for the image.
    // The number of bands is fixed, FFT results are very different for
    // different values but we need some consistency.
    wxSize size = GetClientSize();
    int samples = size.GetWidth() - LPAD - RPAD;
    if (samples > 0) {
        this->image.Create(samples, BANDS);
        this->pipeline = spek_pipeline_open(
            this->path.utf8_str(),
            BANDS,
            samples,
            THRESHOLD,
            pipeline_cb,
            this
        );
        spek_pipeline_start(this->pipeline);
        this->desc = spek_audio_desc(spek_pipeline_properties(this->pipeline));
    } else {
        this->image.Create(1, 1);
    }

    Refresh();
}

// Modified version of Dan Bruton's algorithm:
// http://www.physics.sfasu.edu/astro/color/spectra.html
// TODO: Move out to a C function.
uint32_t SpekSpectrogram::get_color(double level)
{
    level *= 0.6625;
    double r = 0.0, g = 0.0, b = 0.0;
    if (level >= 0 && level < 0.15) {
        r = (0.15 - level) / (0.15 + 0.075);
        g = 0.0;
        b = 1.0;
    } else if (level >= 0.15 && level < 0.275) {
        r = 0.0;
        g = (level - 0.15) / (0.275 - 0.15);
        b = 1.0;
    } else if (level >= 0.275 && level < 0.325) {
        r = 0.0;
        g = 1.0;
        b = (0.325 - level) / (0.325 - 0.275);
    } else if (level >= 0.325 && level < 0.5) {
        r = (level - 0.325) / (0.5 - 0.325);
        g = 1.0;
        b = 0.0;
    } else if (level >= 0.5 && level < 0.6625) {
        r = 1.0;
        g = (0.6625 - level) / (0.6625 - 0.5f);
        b = 0.0;
    }

    // Intensity correction.
    double cf = 1.0;
    if (level >= 0.0 && level < 0.1) {
        cf = level / 0.1;
    }
    cf *= 255.0;

    // Pack RGB values into a 32-bit uint.
    uint32_t rr = (uint32_t) (r * cf + 0.5);
    uint32_t gg = (uint32_t) (g * cf + 0.5);
    uint32_t bb = (uint32_t) (b * cf + 0.5);
    return (rr << 16) + (gg << 8) + bb;
}
