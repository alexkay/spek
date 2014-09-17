/* spek-spectrogram.cc
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

using namespace std;//needed for debugging with cout, FIXME remove all lines with cout after implementing scaling frequency range

#include <cmath>

#include <wx/dcbuffer.h>

#include "spek-audio.h"
#include "spek-events.h"
#include "spek-fft.h"
#include "spek-palette.h"
#include "spek-pipeline.h"
#include "spek-platform.h"
#include "spek-ruler.h"
#include "spek-utils.h"

#include "spek-spectrogram.h"

BEGIN_EVENT_TABLE(SpekSpectrogram, wxWindow)
    EVT_CHAR(SpekSpectrogram::on_char)
    EVT_PAINT(SpekSpectrogram::on_paint)
    EVT_SIZE(SpekSpectrogram::on_size)
    SPEK_EVT_HAVE_SAMPLE(SpekSpectrogram::on_have_sample)
END_EVENT_TABLE()

enum
{
    MAX_RANGE = 0,
    MIN_RANGE = -140,
    MAX_FREQ = 48000,
    MIN_FREQ = 0,
    URANGE = -20,
    LRANGE = -120,
    FFT_BITS = 11,
    BANDS = (1 << (FFT_BITS - 1)) + 1,
    LPAD = 60,
    TPAD = 60,
    RPAD = 90,
    BPAD = 40,
    GAP = 10,
    RULER = 10,
};

// Forward declarations.
static wxString trim(wxDC& dc, const wxString& s, int length, bool trim_end);

SpekSpectrogram::SpekSpectrogram(wxFrame *parent) :
    wxWindow(
        parent, -1, wxDefaultPosition, wxDefaultSize,
        wxFULL_REPAINT_ON_RESIZE | wxWANTS_CHARS
    ),
    audio(new Audio()), // TODO: refactor
    fft(new FFT()),
    pipeline(NULL),
    duration(0.0),
    sample_rate(0),
    palette(RULER, BANDS),
    image(1, 1),
    prev_width(-1),
    urange(URANGE),
    lrange(LRANGE),
    ufreq(MAX_FREQ),
    lfreq(MIN_FREQ)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    SetFocus();

    // Pre-draw the palette.
    for (int y = 0; y < BANDS; y++) {
        uint32_t color = spek_palette_spectrum(y / (double) BANDS);
        this->palette.SetRGB(
            wxRect(0, BANDS - y - 1, RULER, 1),
            color >> 16,
            (color >> 8) & 0xFF,
            color & 0xFF
        );
    }
}

SpekSpectrogram::~SpekSpectrogram()
{
    this->stop();
}

void SpekSpectrogram::open(const wxString& path)
{
    this->path = path;
    start();
    Refresh();
}

void SpekSpectrogram::save(const wxString& path)
{
    wxSize size = GetClientSize();
    wxBitmap bitmap(size.GetWidth(), size.GetHeight());
    wxMemoryDC dc(bitmap);
    render(dc);
    bitmap.SaveFile(path, wxBITMAP_TYPE_PNG);
}

void SpekSpectrogram::on_char(wxKeyEvent& evt)
{
    bool C = evt.GetModifiers() == wxMOD_CONTROL;
    bool CS = evt.GetModifiers() == (wxMOD_CONTROL | wxMOD_SHIFT);
    bool dn = evt.GetKeyCode() == WXK_DOWN;
    bool up = evt.GetKeyCode() == WXK_UP;
    bool lf = evt.GetKeyCode() == WXK_LEFT;
    bool rg = evt.GetKeyCode() == WXK_RIGHT;

    if (C && up) {
        // increasing lower limit of dynamic range
        this->lrange = spek_min(this->lrange + 1, this->urange - 1);
    } else if (C && dn) {
        // decreasing lower limit of dynamic range
        this->lrange = spek_max(this->lrange - 1, MIN_RANGE);
    } else if (CS && up) {
        // increasing upper limit of dynamic range
        this->urange = spek_min(this->urange + 1, MAX_RANGE);
    } else if (CS && dn) {
        // decreasing upper limit of dynamic range
        this->urange = spek_max(this->urange - 1, this->lrange + 1);
    } else if (C && rg) {
        // increasing lower limit frequency range
        cout << "Crg" << this->lfreq << " " << this->ufreq << "\n";//FIXME
        if (this->lfreq == MIN_FREQ) {
            this->lfreq = 1; // because two times zero is still zero
        } else {
            this->lfreq = spek_min(this->lfreq * 2, this->ufreq / 2);
        }
        cout << this->lfreq << "\n";//FIXME
    } else if (C && lf) {
        // decreasing lower limit frequency range
        cout << "Clf" << this->lfreq << " " << this->ufreq << "\n";//FIXME
        if (this->lfreq == MIN_FREQ) {
            // do nothing
        } else if (this->lfreq == 1) {
            this->lfreq = MIN_FREQ; // to avoid going into rounding
        } else {
            this->lfreq = this->lfreq / 2;
        }
        cout << this->lfreq << "\n";//FIXME
    } else if (CS && rg) {
        // increasing upper limit frequency range
        cout << "CSrg" << this->lfreq << " " << this->ufreq << "\n";//FIXME
        if (this->ufreq == MAX_FREQ) {
            // do nothing
        } else {
            this->ufreq = spek_min(this->ufreq * 2, MAX_FREQ);
        }
        cout << this->ufreq << "\n";//FIXME
    } else if (CS && lf) {
        // decreasing upper limit frequency range
        cout << "CSlf" << this->lfreq << " " << this->ufreq << "\n";//FIXME
        this->ufreq = spek_max(this->ufreq / 2, this->lfreq * 2);
        cout << this->ufreq << "\n";//FIXME
    } else {
        evt.Skip();
        return;
    }

    start();
    Refresh();
}

void SpekSpectrogram::on_paint(wxPaintEvent&)
{
    wxAutoBufferedPaintDC dc(this);
    render(dc);
}

void SpekSpectrogram::on_size(wxSizeEvent&)
{
    wxSize size = GetClientSize();
    bool width_changed = this->prev_width != size.GetWidth();
    this->prev_width = size.GetWidth();

    if (width_changed) {
        start();
    }
}

void SpekSpectrogram::on_have_sample(SpekHaveSampleEvent& event)
{
    int bands = event.get_bands();
    int sample = event.get_sample();
    const float *values = event.get_values();

    if (sample == -1) {
        this->stop();
        return;
    }

    // TODO: check image size, quit if wrong.
    double range = this->urange - this->lrange;
    for (int y = 0; y < bands; y++) {
        double value = fmin(this->urange, fmax(this->lrange, values[y]));
        double level = (value - this->lrange) / range;
        uint32_t color = spek_palette_spectrum(level);
        this->image.SetRGB(
            sample,
            bands - y - 1,
            color >> 16,
            (color >> 8) & 0xFF,
            color & 0xFF
        );
    }

    // TODO: refresh only one pixel column
    this->Refresh();
}

static wxString time_formatter(int unit)
{
    // TODO: i18n
    return wxString::Format("%d:%02d", unit / 60, unit % 60);
}

static wxString freq_formatter(int unit)
{
    return wxString::Format(_("%d kHz"), unit / 1000);
}

static wxString density_formatter(int unit)
{
    return wxString::Format(_("%d dB"), -unit);
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
    wxFont normal_font = wxFont(
        (int)round(9 * spek_platform_font_scale()),
        wxFONTFAMILY_SWISS,
        wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_NORMAL
    );
    wxFont large_font = wxFont(normal_font);
    large_font.SetPointSize((int)round(10 * spek_platform_font_scale()));
    large_font.SetWeight(wxFONTWEIGHT_BOLD);
    wxFont small_font = wxFont(normal_font);
    small_font.SetPointSize((int)round(8 * spek_platform_font_scale()));
    dc.SetFont(normal_font);
    int normal_height = dc.GetTextExtent("dummy").GetHeight();
    dc.SetFont(large_font);
    int large_height = dc.GetTextExtent("dummy").GetHeight();
    dc.SetFont(small_font);
    int small_height = dc.GetTextExtent("dummy").GetHeight();

    // Clean the background.
    dc.Clear();

    // Spek version
    dc.SetFont(large_font);
    wxString package_name(PACKAGE_NAME);
    dc.DrawText(
        package_name,
        w - RPAD + GAP,
        TPAD - 2 * GAP - normal_height - large_height
    );
    int package_name_width = dc.GetTextExtent(package_name + " ").GetWidth();
    dc.SetFont(small_font);
    dc.DrawText(
        PACKAGE_VERSION,
        w - RPAD + GAP + package_name_width,
        TPAD - 2 * GAP - normal_height - small_height
    );

    if (this->image.GetWidth() > 1 && this->image.GetHeight() > 1 &&
        w - LPAD - RPAD > 0 && h - TPAD - BPAD > 0) {
        // Draw the spectrogram.
        wxBitmap bmp(this->image.Scale(w - LPAD - RPAD, h - TPAD - BPAD));
        dc.DrawBitmap(bmp, LPAD, TPAD);

        // File name.
        dc.SetFont(large_font);
        dc.DrawText(
            trim(dc, this->path, w - LPAD - RPAD, false),
            LPAD,
            TPAD - 2 * GAP - normal_height - large_height
        );

        // File properties.
        dc.SetFont(normal_font);
        dc.DrawText(
            trim(dc, this->desc, w - LPAD - RPAD, true),
            LPAD,
            TPAD - GAP - normal_height
        );

        // Prepare to draw the rulers.
        dc.SetFont(small_font);

        if (this->duration) {
            // Time ruler.
            int time_factors[] = {1, 2, 5, 10, 20, 30, 1*60, 2*60, 5*60, 10*60, 20*60, 30*60, 0};
            SpekRuler time_ruler(
                LPAD,
                h - BPAD,
                SpekRuler::BOTTOM,
                // TODO: i18n
                "00:00",
                time_factors,
                0,
                (int)this->duration,
                1.5,
                (w - LPAD - RPAD) / this->duration,
                0.0,
                time_formatter
                );
            time_ruler.draw(dc);
        }

        if (this->sample_rate) {
            // Frequency ruler.
            int freq = this->sample_rate / 2;
            int freq_factors[] = {1000, 2000, 5000, 10000, 20000, 0};
            SpekRuler freq_ruler(
                LPAD,
                TPAD,
                SpekRuler::LEFT,
                // TRANSLATORS: keep "00" unchanged, it's used to calc the text width
                _("00 kHz"),
                freq_factors,
                0,//FIXME replace with this->lfreq?
                freq,//FIXME replace with this->ufreq?
                3.0,
                (h - TPAD - BPAD) / (double)freq,//FIXME replace with (double)this->ufreq
                0.0,//FIXME replace with (double)this->lfreq ?
                freq_formatter
                );
            freq_ruler.draw(dc);
        }
    }

    // Border around the spectrogram.
    dc.DrawRectangle(LPAD, TPAD, w - LPAD - RPAD, h - TPAD - BPAD);

    // The palette.
    if (h - TPAD - BPAD > 0) {
        wxBitmap bmp(this->palette.Scale(RULER, h - TPAD - BPAD + 1));
        dc.DrawBitmap(bmp, w - RPAD + GAP, TPAD);

        // Prepare to draw the ruler.
        dc.SetFont(small_font);

        // Spectral density.
        int density_factors[] = {1, 2, 5, 10, 20, 50, 0};
        SpekRuler density_ruler(
            w - RPAD + GAP + RULER,
            TPAD,
            SpekRuler::RIGHT,
            // TRANSLATORS: keep "-00" unchanged, it's used to calc the text width
            _("-00 dB"),
            density_factors,
            -this->urange,
            -this->lrange,
            3.0,
            (h - TPAD - BPAD) / (double)(this->lrange - this->urange),
            h - TPAD - BPAD,
            density_formatter
        );
        density_ruler.draw(dc);
    }
}

static void pipeline_cb(int sample, float *values, void *cb_data)
{
    SpekHaveSampleEvent event(BANDS, sample, values, false);
    SpekSpectrogram *s = (SpekSpectrogram *)cb_data;
    wxPostEvent(s, event);
}

void SpekSpectrogram::start()
{
    if (this->path.IsEmpty()) {
        return;
    }

    this->stop();

    // The number of samples is the number of pixels available for the image.
    // The number of bands is fixed, FFT results are very different for
    // different values but we need some consistency.
    wxSize size = GetClientSize();
    int samples = size.GetWidth() - LPAD - RPAD;
    if (samples > 0) {
        this->image.Create(samples, BANDS);
        this->pipeline = spek_pipeline_open(
            this->audio->open(std::string(this->path.utf8_str())),
            this->fft->create(FFT_BITS),
            samples,
            pipeline_cb,
            this
        );
        spek_pipeline_start(this->pipeline);
        // TODO: extract conversion into a utility function.
        this->desc = wxString::FromUTF8(spek_pipeline_desc(this->pipeline).c_str());
        this->duration = spek_pipeline_duration(this->pipeline);
        this->sample_rate = spek_pipeline_sample_rate(this->pipeline);
        this->ufreq = this->sample_rate / 2;
    } else {
        this->image.Create(1, 1);
    }
}

void SpekSpectrogram::stop()
{
    if (this->pipeline) {
        spek_pipeline_close(this->pipeline);
        this->pipeline = NULL;

        // Make sure all have_sample events are processed before returning.
        wxApp::GetInstance()->ProcessPendingEvents();
    }
}

// Trim `s` so that it fits into `length`.
static wxString trim(wxDC& dc, const wxString& s, int length, bool trim_end)
{
    if (length <= 0) {
        return wxEmptyString;
    }

    // Check if the entire string fits.
    wxSize size = dc.GetTextExtent(s);
    if (size.GetWidth() <= length) {
        return s;
    }

    // Binary search FTW!
    wxString fix("...");
    int i = 0;
    int k = s.length();
    while (k - i > 1) {
        int j = (i + k) / 2;
        size = dc.GetTextExtent(trim_end ? s.substr(0, j) + fix : fix + s.substr(j));
        if (trim_end != (size.GetWidth() > length)) {
            i = j;
        } else {
            k = j;
        }
    }

    return trim_end ? s.substr(0, i) + fix : fix + s.substr(k);
}
