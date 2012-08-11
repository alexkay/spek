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

#include <wx/artprov.h>

#include "spek-window.hh"

BEGIN_EVENT_TABLE(SpekWindow, wxFrame)
    EVT_MENU(wxID_OPEN, SpekWindow::OnOpen)
    EVT_MENU(wxID_SAVE, SpekWindow::OnSave)
    EVT_MENU(wxID_EXIT, SpekWindow::OnExit)
    EVT_MENU(wxID_PREFERENCES, SpekWindow::OnPreferences)
    EVT_MENU(wxID_ABOUT, SpekWindow::OnAbout)
END_EVENT_TABLE()

SpekWindow::SpekWindow(const wxString& title) : wxFrame(NULL, -1, title)
{
    // TODO: test on all platforms
    SetIcon(wxIcon(wxT("spek")));

    wxMenuBar *menu = new wxMenuBar();

    wxMenu *menu_file = new wxMenu();
    wxMenuItem *menu_file_open = new wxMenuItem(menu_file, wxID_OPEN);
    menu_file->Append(menu_file_open);
    wxMenuItem *menu_file_save = new wxMenuItem(menu_file, wxID_SAVE);
    menu_file->Append(menu_file_save);
    menu_file->AppendSeparator();
    menu_file->Append(wxID_EXIT);
    menu->Append(menu_file, _("&File")); // TODO: stock text

    wxMenu *menu_edit = new wxMenu();
    wxMenuItem *menu_edit_prefs = new wxMenuItem(menu_edit, wxID_PREFERENCES);
    // TODO: check if this is needed
    menu_edit_prefs->SetItemLabel(menu_edit_prefs->GetItemLabelText() + wxT("\tCtrl+E"));
    menu_edit->Append(menu_edit_prefs);
    menu->Append(menu_edit, _("&Edit")); // TODO: stock text

    wxMenu *menu_help = new wxMenu();
    wxMenuItem *menu_help_about = new wxMenuItem(menu_help, wxID_ABOUT);
    // TODO: check if this is needed
    menu_help_about->SetItemLabel(menu_help_about->GetItemLabelText() + wxT("\tF1"));
    menu_help->Append(menu_help_about);
    menu->Append(menu_help, _("&Help")); // TODO: stock text

    SetMenuBar(menu);

    wxToolBar *toolbar = CreateToolBar();
    // TODO: bundled file open/save icons suck, provide our own (tango?)
    toolbar->AddTool(
        wxID_OPEN,
        menu_file_open->GetItemLabelText(),
        wxArtProvider::GetBitmap(wxART_FILE_OPEN)
    );
    toolbar->AddTool(
        wxID_SAVE,
        menu_file_save->GetItemLabelText(),
        wxArtProvider::GetBitmap(wxART_FILE_SAVE)
    );
    toolbar->Realize();
}

// TODO: s/audio/media/
static const char *audio_extensions[] = {
    "3gp",
    "aac",
    "aif",
    "aifc",
    "aiff",
    "amr",
    "awb",
    "ape",
    "au",
    "dts",
    "flac",
    "gsm",
    "m4a",
    "m4p",
    "mp3",
    "mp4",
    "mp+",
    "mpc",
    "mpp",
    "oga",
    "ogg",
    "ra",
    "ram",
    "snd",
    "wav",
    "wma",
    "wv",
    NULL
};

void SpekWindow::OnOpen(wxCommandEvent& WXUNUSED(event))
{
    static wxString cur_dir = wxGetHomeDir();
    static wxString filters = wxEmptyString;
    static int filter_index = 1;

    if (filters.IsEmpty()) {
        filters.Alloc(1024);
        filters += _("All files");
        filters += wxT("|*.*|");
        filters += _("Audio files");
        filters += wxT("|");
        for (int i = 0; audio_extensions[i]; ++i) {
            if (i) {
                filters += wxT(";");
            }
            filters += wxT("*.");
            filters += wxString::FromAscii(audio_extensions[i]);
        }
        filters.Shrink();
    }

    wxFileDialog *dlg = new wxFileDialog(
        this,
        _("Open File"),
        cur_dir,
        wxEmptyString,
        filters,
        wxFD_OPEN
    );
    dlg->SetFilterIndex(filter_index);

    if (dlg->ShowModal() == wxID_OK) {
        cur_dir = dlg->GetDirectory();
        filter_index = dlg->GetFilterIndex();
        Open(dlg->GetPath());
    }

    dlg->Destroy();
}

void SpekWindow::OnSave(wxCommandEvent& WXUNUSED(event))
{
}

void SpekWindow::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void SpekWindow::OnPreferences(wxCommandEvent& WXUNUSED(event))
{
}

void SpekWindow::OnAbout(wxCommandEvent& WXUNUSED(event))
{
}

void SpekWindow::Open(const wxString& path)
{
}
