// -*-c++-*-

#ifndef SPEK_WINDOW_H_
#define SPEK_WINDOW_H_

#include <wx/wx.h>

class SpekWindow : public wxFrame
{
public:
    SpekWindow(const wxString& title, const wxPoint& pos, const wxSize& size);

protected:
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

private:
    DECLARE_EVENT_TABLE()
};

#endif
