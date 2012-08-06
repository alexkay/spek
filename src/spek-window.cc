/* spek-window.cc
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

#include "spek-window.hh"

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
