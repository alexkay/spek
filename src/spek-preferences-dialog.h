#pragma once

#include <wx/wx.h>

class SpekPreferencesDialog : public wxDialog
{
public:
    SpekPreferencesDialog(wxWindow *parent);

private:
    void on_language(wxCommandEvent& event);
    void on_check(wxCommandEvent& event);

    wxArrayString languages;

    DECLARE_EVENT_TABLE()
};
