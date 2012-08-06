#include <wx/wx.h>

#include "spek-window.h"

class Spek: public wxApp
{
    virtual bool OnInit();
};

IMPLEMENT_APP(Spek)

bool Spek::OnInit()
{
    SpekWindow *window = new SpekWindow(wxT("Hello World"), wxPoint(50,50), wxSize(450,340));
    window->Show(true);
    SetTopWindow(window);
    return true;
}
