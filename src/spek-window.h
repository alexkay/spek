#pragma once

#include <wx/wx.h>

class SpekSpectrogram;

class SpekWindow : public wxFrame
{
public:
    SpekWindow(const wxString& path);
    void open(const wxString& path);

private:
    void on_open(wxCommandEvent& event);
    void on_save(wxCommandEvent& event);
    void on_exit(wxCommandEvent& event);
    void on_preferences(wxCommandEvent& event);
    void on_help(wxCommandEvent& event);
    void on_about(wxCommandEvent& event);
    void on_notify(wxCommandEvent& event);
    void on_visit(wxCommandEvent& event);
    void on_close(wxCommandEvent& event);

    SpekSpectrogram *spectrogram;
    wxString path;
    wxString cur_dir;
    wxString description;

    DECLARE_EVENT_TABLE()
};
