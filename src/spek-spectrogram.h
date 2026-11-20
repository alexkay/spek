#pragma once

#include <memory>

#include <wx/wx.h>

#include "spek-palette.h"
#include "spek-pipeline.h"

class Audio;
class FFT;
class SpekHaveSampleEvent;
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
    void on_paint(wxPaintEvent& evt);
    void on_size(wxSizeEvent& evt);
    void on_mouse_wheel(wxMouseEvent& evt);
    void on_left_down(wxMouseEvent& evt);
    void on_left_up(wxMouseEvent& evt);
    void on_motion(wxMouseEvent& evt);
    void set_overview_position(int x);
    void on_left_dclick(wxMouseEvent& evt);
    void on_have_sample(SpekHaveSampleEvent& evt);
    void render(wxDC& dc);

    void start();
    void stop();

    void create_palette();

    std::unique_ptr<Audio> audio;
    std::unique_ptr<FFT> fft;
    spek_pipeline *pipeline;
    int streams;
    int stream;
    int channels;
    int channel;
    enum window_function window_function;
    wxString path;
    wxString desc;
    double duration;
    double view_start;
    double view_duration;
    bool dragging;
    bool overview_dragging;
    int drag_start_x;
    double drag_start_view;
    int drag_offset_x;
    int overview_x;
    int overview_y;
    int overview_width;
    int overview_height;
    int overview_thumb_x;
    int overview_thumb_width;
    int overview_grab_offset;
    int sample_rate;
    enum palette palette;
    wxImage palette_image;
    wxImage image;
    int prev_width;
    int fft_bits;
    int urange;
    int lrange;
    const int LPAD;
    const int TPAD;
    const int RPAD;
    const int BPAD;
    const int GAP;
    const int RULER;

    DECLARE_EVENT_TABLE()
};
