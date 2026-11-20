#include <cmath>

#include <wx/dcbuffer.h>

#include "spek-audio.h"
#include "spek-events.h"
#include "spek-fft.h"
#include "spek-platform.h"
#include "spek-ruler.h"
#include "spek-utils.h"

#include "spek-spectrogram.h"

BEGIN_EVENT_TABLE(SpekSpectrogram, wxWindow)
    EVT_CHAR(SpekSpectrogram::on_char)
    EVT_PAINT(SpekSpectrogram::on_paint)
    EVT_SIZE(SpekSpectrogram::on_size)
    EVT_MOUSEWHEEL(SpekSpectrogram::on_mouse_wheel)
    EVT_LEFT_DOWN(SpekSpectrogram::on_left_down)
    EVT_LEFT_UP(SpekSpectrogram::on_left_up)
    EVT_MOTION(SpekSpectrogram::on_motion)
    EVT_LEFT_DCLICK(SpekSpectrogram::on_left_dclick)
END_EVENT_TABLE()

enum
{
    MIN_RANGE = -140,
    MAX_RANGE = 0,
    URANGE = 0,
    LRANGE = -120,
    FFT_BITS = 11,
    MIN_FFT_BITS = 8,
    MAX_FFT_BITS = 14,
};

// Forward declarations.
static wxString trim(wxDC& dc, const wxString& s, int length, bool trim_end);
static int bits_to_bands(int bits);

SpekSpectrogram::SpekSpectrogram(wxFrame *parent) :
    wxWindow(
        parent, -1, wxDefaultPosition, wxDefaultSize,
        wxFULL_REPAINT_ON_RESIZE | wxWANTS_CHARS
    ),
    audio(new Audio()), // TODO: refactor
    fft(new FFT()),
    pipeline(NULL),
    streams(0),
    stream(0),
    channels(0),
    channel(0),
    window_function(WINDOW_DEFAULT),
    duration(0.0),
    view_start(0.0),
    view_duration(0.0),
    dragging(false),
    overview_dragging(false),
    drag_start_x(0),
    drag_start_view(0.0),
    drag_offset_x(0),
    overview_x(0),
    overview_y(0),
    overview_width(0),
    overview_height(0),
    overview_thumb_x(0),
    overview_thumb_width(0),
    overview_grab_offset(0),
    sample_rate(0),
    palette(PALETTE_DEFAULT),
    palette_image(),
    image(1, 1),
    prev_width(-1),
    fft_bits(FFT_BITS),
    urange(URANGE),
    lrange(LRANGE),
    LPAD(this->FromDIP(60)),
    TPAD(this->FromDIP(60)),
    RPAD(this->FromDIP(90)),
    BPAD(this->FromDIP(40)),
    GAP(this->FromDIP(10)),
    RULER(this->FromDIP(10))
{
    this->create_palette();

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    SetFocus();
    Bind(SPEK_HAVE_SAMPLE, &SpekSpectrogram::on_have_sample, this);
}

SpekSpectrogram::~SpekSpectrogram()
{
    this->stop();
}

void SpekSpectrogram::open(const wxString& path)
{
    this->path = path;
    this->view_start = 0.0;
    this->view_duration = 0.0;
    this->stream = 0;
    this->channel = 0;
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
    switch (evt.GetKeyCode()) {
    case 'c':
        if (this->channels) {
            this->channel = (this->channel + 1) % this->channels;
        }
        break;
    case 'C':
        if (this->channels) {
            this->channel = (this->channel - 1 + this->channels) % this->channels;
        }
        break;
    case 'f':
        this->window_function = (enum window_function) ((this->window_function + 1) % WINDOW_COUNT);
        break;
    case 'F':
        this->window_function =
            (enum window_function) ((this->window_function - 1 + WINDOW_COUNT) % WINDOW_COUNT);
        break;
    case 'l':
        this->lrange = spek_min(this->lrange + 1, this->urange - 1);
        break;
    case 'L':
        this->lrange = spek_max(this->lrange - 1, MIN_RANGE);
        break;
    case 'p':
        this->palette = (enum palette) ((this->palette + 1) % PALETTE_COUNT);
        this->create_palette();
        break;
    case 'P':
        this->palette = (enum palette) ((this->palette - 1 + PALETTE_COUNT) % PALETTE_COUNT);
        this->create_palette();
        break;
    case 's':
        if (this->streams) {
            this->stream = (this->stream + 1) % this->streams;
            this->view_start = 0.0;
            this->view_duration = 0.0;
        }
        break;
    case 'S':
        if (this->streams) {
            this->stream = (this->stream - 1 + this->streams) % this->streams;
            this->view_start = 0.0;
            this->view_duration = 0.0;
        }
        break;
    case 'u':
        this->urange = spek_min(this->urange + 1, MAX_RANGE);
        break;
    case 'U':
        this->urange = spek_max(this->urange - 1, this->lrange + 1);
        break;
    case 'w':
        this->fft_bits = spek_min(this->fft_bits + 1, MAX_FFT_BITS);
        this->create_palette();
        break;
    case 'W':
        this->fft_bits = spek_max(this->fft_bits - 1, MIN_FFT_BITS);
        this->create_palette();
        break;
    default:
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

    const double max_start = this->duration - this->view_duration;
    const int travel = this->overview_width - 2 - this->overview_thumb_width;
    const bool can_snap_edge =
        this->duration > 0.0 && this->view_duration > 0.0 &&
        max_start > 0.0 && travel > 0;
    const bool keep_left_edge =
        can_snap_edge && this->view_start <= max_start / travel;
    const bool keep_right_edge =
        can_snap_edge &&
        max_start - this->view_start <= max_start / travel;

    if (width_changed) {
        start();
    }

    if (this->view_duration < this->duration &&
        (keep_left_edge || keep_right_edge)) {
        this->view_start = keep_left_edge
            ? 0.0 : this->duration - this->view_duration;
        Refresh();
    }
}

void SpekSpectrogram::on_mouse_wheel(wxMouseEvent& event)
{
    if (this->duration <= 0.0 || this->view_duration <= 0.0) {
        event.Skip();
        return;
    }

    wxPoint point = event.GetPosition();
    int plot_width = GetClientSize().GetWidth() - LPAD - RPAD;
    if (plot_width <= 0 || point.x < LPAD || point.x > LPAD + plot_width ||
        point.y < TPAD || point.y > GetClientSize().GetHeight() - BPAD) {
        event.Skip();
        return;
    }

    double notches = event.GetWheelRotation() / (double)event.GetWheelDelta();
    if (notches == 0.0) {
        event.Skip();
        return;
    }

    if (event.ShiftDown()) {
        this->view_start -= notches * this->view_duration * 0.15;
    } else {
        const double fraction = (point.x - LPAD) / (double)plot_width;
        const double anchor = this->view_start + fraction * this->view_duration;
        const double next_duration = spek_min(
            this->duration,
            spek_max(1.0, this->view_duration * pow(2.0, -notches))
        );
        this->view_start = anchor - fraction * next_duration;
        this->view_duration = next_duration;
    }

    if (this->view_duration >= this->duration) {
        this->view_start = 0.0;
        this->view_duration = this->duration;
    } else {
        this->view_start = spek_max(
            0.0, spek_min(this->view_start, this->duration - this->view_duration)
        );
    }

    start();
    Refresh();
}

void SpekSpectrogram::on_left_down(wxMouseEvent& event)
{
    wxPoint point = event.GetPosition();
    if (this->duration > 0.0 && this->view_duration < this->duration &&
        this->overview_width > 0 &&
        point.x >= this->overview_x && point.x < this->overview_x + this->overview_width &&
        point.y >= this->overview_y && point.y < this->overview_y + this->overview_height) {
        this->overview_grab_offset =
            point.x >= this->overview_thumb_x &&
            point.x < this->overview_thumb_x + this->overview_thumb_width
            ? point.x - this->overview_thumb_x
            : this->overview_thumb_width / 2;
        this->overview_dragging = true;
        this->drag_start_x = point.x;
        this->drag_start_view = this->view_start;
        SetFocus();
        CaptureMouse();
        SetCursor(wxCursor(wxCURSOR_HAND));
        set_overview_position(point.x);
        return;
    }

    if (this->duration <= 0.0 || this->view_duration >= this->duration) {
        event.Skip();
        return;
    }

    int plot_width = GetClientSize().GetWidth() - LPAD - RPAD;
    if (plot_width <= 0 || point.x < LPAD || point.x > LPAD + plot_width ||
        point.y < TPAD || point.y > GetClientSize().GetHeight() - BPAD) {
        event.Skip();
        return;
    }

    this->dragging = true;
    this->drag_start_x = point.x;
    this->drag_start_view = this->view_start;
    this->drag_offset_x = 0;
    SetFocus();
    CaptureMouse();
    SetCursor(wxCursor(wxCURSOR_HAND));
}

void SpekSpectrogram::on_left_up(wxMouseEvent& event)
{
    if (this->overview_dragging) {
        // Apply the final pointer position before ending the drag.
        set_overview_position(event.GetX());

        // Preserve an intentional right-edge landing across start(), which
        // reopens the audio pipeline and re-clamps view_start.
        const int thumb_right = this->overview_thumb_x + this->overview_thumb_width;
        const int track_right = this->overview_x + this->overview_width - 1;
        const bool finish_at_right_edge =
            this->overview_width > 0 && track_right - thumb_right <= 1;

        this->overview_dragging = false;
        if (HasCapture()) {
            ReleaseMouse();
        }
        SetCursor(wxCursor(wxCURSOR_ARROW));
        this->drag_offset_x = 0;
        start();
        if (finish_at_right_edge && this->view_duration < this->duration) {
            this->view_start = this->duration - this->view_duration;
        }
        Refresh();
        return;
    }

    if (!this->dragging) {
        event.Skip();
        return;
    }

    this->dragging = false;
    if (HasCapture()) {
        ReleaseMouse();
    }
    SetCursor(wxCursor(wxCURSOR_ARROW));

    bool moved = this->view_start != this->drag_start_view;
    this->drag_offset_x = 0;
    if (moved) {
        start();
        Refresh();
    }
}

void SpekSpectrogram::on_motion(wxMouseEvent& event)
{
    if (this->overview_dragging) {
        set_overview_position(event.GetX());
        Update();
        return;
    }

    if (!this->dragging) {
        event.Skip();
        return;
    }

    int plot_width = GetClientSize().GetWidth() - LPAD - RPAD;
    if (plot_width <= 0 || this->view_duration <= 0.0) {
        return;
    }

    int delta_x = event.GetX() - this->drag_start_x;
    double next_start = this->drag_start_view -
        delta_x * this->view_duration / plot_width;
    this->view_start = spek_max(
        0.0, spek_min(next_start, this->duration - this->view_duration)
    );
    this->drag_offset_x = (int)round(
        (this->drag_start_view - this->view_start) * plot_width / this->view_duration
    );
    Refresh();
    Update();
}

void SpekSpectrogram::set_overview_position(int x)
{
    int inner_width = this->overview_width - 2;
    int travel = inner_width - this->overview_thumb_width;
    double max_start = this->duration - this->view_duration;
    if (inner_width <= 0 || travel <= 0 || max_start <= 0.0) {
        return;
    }

    int thumb_x = x - this->overview_grab_offset;
    thumb_x = spek_max(this->overview_x + 1,
        spek_min(thumb_x, this->overview_x + 1 + travel));
    double position = (thumb_x - this->overview_x - 1) / (double)travel;
    this->view_start = position * max_start;

    int plot_width = GetClientSize().GetWidth() - LPAD - RPAD;
    if (plot_width > 0 && this->view_duration > 0.0) {
        this->drag_offset_x = (int)round(
            (this->drag_start_view - this->view_start) * plot_width / this->view_duration
        );
    }
    Refresh();
    Update();
}

void SpekSpectrogram::on_left_dclick(wxMouseEvent& event)
{
    wxPoint point = event.GetPosition();
    int plot_width = GetClientSize().GetWidth() - LPAD - RPAD;
    if (point.x >= LPAD && point.x <= LPAD + plot_width && this->duration > 0.0) {
        this->view_start = 0.0;
        this->view_duration = this->duration;
        start();
        Refresh();
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
        uint32_t color = spek_palette(this->palette, level);
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
    this->overview_width = 0;
    this->overview_height = 0;
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
        dc.SetClippingRegion(LPAD, TPAD, w - LPAD - RPAD, h - TPAD - BPAD);
        dc.DrawBitmap(bmp, LPAD + this->drag_offset_x, TPAD);
        dc.DestroyClippingRegion();

        const int plot_width = w - LPAD - RPAD;
        const int overview_width = FromDIP(90);
        const int overview_height = FromDIP(8);
        const bool show_overview =
            this->duration > 0.0 && this->view_duration > 0.0 &&
            plot_width >= overview_width + FromDIP(120);
        const int overview_x = w - RPAD - overview_width;
        const int overview_y =
            TPAD - GAP - small_height - overview_height - FromDIP(2);

        // File name.
        dc.SetFont(large_font);
        dc.DrawText(
            trim(dc, this->path, plot_width, false),
            LPAD,
            TPAD - 2 * GAP - normal_height - large_height
        );

        // File properties.
        dc.SetFont(normal_font);
        int desc_width = show_overview ? overview_x - LPAD - GAP : plot_width;
        dc.DrawText(
            trim(dc, this->desc, desc_width, true),
            LPAD,
            TPAD - GAP - normal_height
        );

        if (show_overview) {
            int zoom_tenths = (int)round(10.0 * this->duration / this->view_duration);
            wxString zoom_label = wxString::Format(
                "%d.%dx", zoom_tenths / 10, zoom_tenths % 10
            );

            const int inner_width = overview_width - 2;
            const int inner_height = overview_height - 2;
            const double max_start = this->duration - this->view_duration;
            const double edge_tolerance =
                1e-9 * spek_max(1.0, this->duration);
            const bool at_start = this->view_start <= edge_tolerance;
            const bool at_end = max_start - this->view_start <= edge_tolerance;

            // Keep the thumb width independent of its position. The drag
            // mapping uses this same width to calculate its available travel.
            const int min_thumb_width = spek_min(FromDIP(3), inner_width);
            int thumb_width = (int)round(
                inner_width * this->view_duration / this->duration
            );
            thumb_width = spek_max(
                min_thumb_width, spek_min(thumb_width, inner_width)
            );
            if (max_start > 0.0 && inner_width > 1) {
                thumb_width = spek_min(thumb_width, inner_width - 1);
            }

            const int travel = inner_width - thumb_width;
            int thumb_left = max_start > 0.0
                ? (int)round(travel * this->view_start / max_start)
                : 0;
            if (at_start) {
                thumb_left = 0;
            } else if (at_end) {
                thumb_left = travel;
            } else {
                thumb_left = spek_max(0, spek_min(thumb_left, travel));
            }
            int thumb_x = overview_x + 1 + thumb_left;
            this->overview_x = overview_x;
            this->overview_y = overview_y;
            this->overview_width = overview_width;
            this->overview_height = overview_height;
            this->overview_thumb_x = thumb_x;
            this->overview_thumb_width = thumb_width;

            // Draw the minimap outline.
            dc.SetPen(wxPen(wxColour(255, 255, 255), FromDIP(1)));
            dc.SetBrush(wxBrush(wxColour(16, 16, 16)));
            dc.DrawRectangle(overview_x, overview_y, overview_width, overview_height);

            dc.SetPen(wxPen(wxColour(245, 245, 245)));
            dc.SetBrush(wxBrush(wxColour(245, 245, 245)));
            dc.DrawRectangle(
                thumb_x, overview_y + 1,
                thumb_width + (at_end ? 1 : 0), inner_height
            );

            // Redraw the outline over the thumb so the viewport meets the
            // right border cleanly despite wxDC rectangle edge conventions.
            dc.SetPen(wxPen(wxColour(255, 255, 255), FromDIP(1)));
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            dc.DrawRectangle(overview_x, overview_y, overview_width, overview_height);

            dc.SetFont(small_font);
            int label_width = dc.GetTextExtent(zoom_label).GetWidth();
            dc.DrawText(
                zoom_label,
                overview_x + (overview_width - label_width) / 2,
                overview_y + overview_height + FromDIP(1)
            );

        }

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
                (int)floor(this->view_start),
                (int)ceil(this->view_start + this->view_duration),
                1.5,
                (w - LPAD - RPAD) / this->view_duration,
                (floor(this->view_start) - this->view_start) *
                    (w - LPAD - RPAD) / this->view_duration,
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
                0,
                freq,
                3.0,
                (h - TPAD - BPAD) / (double)freq,
                0.0,
                freq_formatter
                );
            freq_ruler.draw(dc);
        }
    }

    // Border around the spectrogram. The overview thumb leaves a solid brush
    // selected, so restore the transparent brush before drawing this rectangle.
    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(LPAD, TPAD, w - LPAD - RPAD, h - TPAD - BPAD);

    // The palette.
    if (h - TPAD - BPAD > 0) {
        wxBitmap bmp(this->palette_image.Scale(RULER, h - TPAD - BPAD + 1));
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

static void pipeline_cb(int bands, int sample, float *values, void *cb_data)
{
    SpekHaveSampleEvent event(bands, sample, values, false);
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
        this->image.Create(samples, bits_to_bands(this->fft_bits));
        this->pipeline = spek_pipeline_open(
            this->audio->open(std::string(this->path.utf8_str()), this->stream),
            this->fft->create(this->fft_bits),
            this->stream,
            this->channel,
            this->window_function,
            samples,
            this->view_start,
            this->view_duration,
            pipeline_cb,
            this
        );
        // TODO: extract conversion into a utility function.
        this->desc = wxString::FromUTF8(spek_pipeline_desc(this->pipeline).c_str());
        this->streams = spek_pipeline_streams(this->pipeline);
        this->channels = spek_pipeline_channels(this->pipeline);
        this->duration = spek_pipeline_duration(this->pipeline);
        this->sample_rate = spek_pipeline_sample_rate(this->pipeline);
        if (this->view_duration <= 0.0 || this->view_duration > this->duration) {
            this->view_start = 0.0;
            this->view_duration = this->duration;
        } else {
            this->view_start = spek_max(
                0.0, spek_min(this->view_start, this->duration - this->view_duration)
            );
        }
        spek_pipeline_start(this->pipeline);
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

void SpekSpectrogram::create_palette()
{
    this->palette_image.Create(RULER, bits_to_bands(this->fft_bits));
    for (int y = 0; y < bits_to_bands(this->fft_bits); y++) {
        uint32_t color = spek_palette(this->palette, y / (double)bits_to_bands(this->fft_bits));
        this->palette_image.SetRGB(
            wxRect(0, bits_to_bands(this->fft_bits) - y - 1, RULER, 1),
            color >> 16,
            (color >> 8) & 0xFF,
            color & 0xFF
        );
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

// TODO: test
static int bits_to_bands(int bits) {
    return (1 << (bits - 1)) + 1;
}
