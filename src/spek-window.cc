#include "spek-window.h"

enum
{
    ID_Quit = 1,
    ID_About,
};

BEGIN_EVENT_TABLE(SpekWindow, wxFrame)
    EVT_MENU(ID_Quit,  SpekWindow::OnQuit)
    EVT_MENU(ID_About, SpekWindow::OnAbout)
END_EVENT_TABLE()

SpekWindow::SpekWindow(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, -1, title, pos, size)
{
    wxMenu *menuFile = new wxMenu();

    menuFile->Append(ID_About, wxT("&About..."));
    menuFile->AppendSeparator();
    menuFile->Append(ID_Quit, wxT("E&xit"));

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, wxT("&File"));

    SetMenuBar(menuBar);
}

void SpekWindow::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void SpekWindow::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(
        wxT("This is a wxWidgets' Hello world sample"),
        wxT("About Hello World"),
        wxOK | wxICON_INFORMATION
    );
}
