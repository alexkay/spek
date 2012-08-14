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

#include <wx/dcbuffer.h>

#include "spek-spectrogram.hh"

BEGIN_EVENT_TABLE(SpekSpectrogram, wxPanel)
    EVT_PAINT(SpekSpectrogram::OnPaint)
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
    palette(RULER, BANDS)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    // Pre-draw the palette.
    for (int y = 0; y < BANDS; y++) {
        uint32_t color = GetColor(y / (double) BANDS);
        this->palette.SetRGB(
            wxRect(0, BANDS - y - 1, RULER, 1),
            color >> 16,
            (color >> 8) & 0xFF,
            color & 0xFF
        );
    }
}

void SpekSpectrogram::Open(const wxString& path)
{
    this->path = path;
    Start();
}

void SpekSpectrogram::Save(const wxString& path)
{
}

void SpekSpectrogram::OnPaint(wxPaintEvent& evt)
{
    wxAutoBufferedPaintDC dc(this);
    Render(dc);
}

void SpekSpectrogram::Render(wxDC& dc)
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
    wxFont normal = wxFont(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    wxFont large = wxFont(normal);
    large.SetPointSize(10);
    large.SetWeight(wxFONTWEIGHT_BOLD);
    wxFont small = wxFont(normal);
    small.SetPointSize(8);
    dc.SetFont(normal);
    int normal_height = dc.GetTextExtent(wxT("dummy")).GetHeight();
    dc.SetFont(large);
    int large_height = dc.GetTextExtent(wxT("dummy")).GetHeight();

    // Clean the background.
    dc.Clear();

    // Spek version
    dc.SetFont(large);
    wxString package_name(wxT(PACKAGE_NAME));
    dc.DrawText(
        package_name,
        w - RPAD + GAP,
        TPAD - 2 * GAP - normal_height - large_height
    );
    int package_name_width = dc.GetTextExtent(package_name + wxT(" ")).GetWidth();
    dc.SetFont(normal);
    dc.DrawText(
        wxT(PACKAGE_VERSION),
        w - RPAD + GAP + package_name_width,
        TPAD - 2 * GAP - 2 * normal_height
    );

    // Border around the spectrogram.
    // TODO: check if this uses antialiasing
    dc.DrawRectangle(LPAD, TPAD, w - LPAD - RPAD, h - TPAD - BPAD);

    // The palette.
    wxBitmap bmp(this->palette.Scale(RULER, h - TPAD - BPAD + 1 /*TODO:, wxIMAGE_QUALITY_HIGH*/));
    dc.DrawBitmap(bmp, w - RPAD + GAP, TPAD);
}

void SpekSpectrogram::Start()
{
}

// Modified version of Dan Bruton's algorithm:
// http://www.physics.sfasu.edu/astro/color/spectra.html
// TODO: Move out to a C function.
uint32_t SpekSpectrogram::GetColor(double level)
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
