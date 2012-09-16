/* spek.cc
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

#include <wx/cmdline.h>
#include <wx/log.h>
#include <wx/socket.h>

extern "C" {
#include <spek-audio.h>
}

#include "spek-artwork.hh"
#include "spek-platform.hh"
#include "spek-preferences.hh"

#include "spek-window.hh"

class Spek: public wxApp
{
public:
    Spek() : wxApp(), window(NULL), quit(false) {}

protected:
    virtual bool OnInit();
    virtual int OnRun();
#ifdef OS_OSX
    virtual void MacOpenFiles(const wxArrayString& files);
#endif

private:
    SpekWindow *window;
    wxString path;
    bool quit;
};

IMPLEMENT_APP(Spek)

bool Spek::OnInit()
{
    wxInitAllImageHandlers();
    wxSocketBase::Initialize();

    spek_audio_init();

    spek_artwork_init();
    spek_platform_init();
    SpekPreferences::get().init();

    static const wxCmdLineEntryDesc desc[] = {{
            wxCMD_LINE_SWITCH,
            wxT_2("h"),
            wxT_2("help"),
            wxT_2("Show this help message"),
            wxCMD_LINE_VAL_NONE,
            wxCMD_LINE_OPTION_HELP
        }, {
            wxCMD_LINE_SWITCH,
            wxT_2("V"),
            wxT_2("version"),
            wxT_2("Display the version and exit")
        }, {
            wxCMD_LINE_PARAM,
            NULL,
            NULL,
            wxT_2("FILE"),
            wxCMD_LINE_VAL_STRING,
            wxCMD_LINE_PARAM_OPTIONAL
        }, {
            // TODO: use wxCMD_LINE_DESC_END after settling on wx29.
            wxCMD_LINE_NONE, NULL, NULL, NULL, wxCMD_LINE_VAL_NONE, 0
        }
    };

    wxCmdLineParser parser(desc, argc, argv);
    int ret = parser.Parse(true);
    if (ret == 1) {
        return false;
    }
    if (ret == -1) {
        this->quit = true;
        return true;
    }
    if (parser.Found(wxT("version"))) {
        // TRANSLATORS: the %s is the package version.
        wxPrintf(_("Spek version %s"), wxT(PACKAGE_VERSION));
        wxPrintf(wxT("\n"));
        this->quit = true;
        return true;
    }
    if (parser.GetParamCount()) {
        this->path = parser.GetParam();
    }

    this->window = new SpekWindow(this->path);
    this->window->Show(true);
    SetTopWindow(this->window);
    return true;
}

int Spek::OnRun()
{
    if (quit) {
        return 0;
    }

    return wxApp::OnRun();
}

#ifdef OS_OSX
void Spek::MacOpenFiles(const wxArrayString& files)
{
    if (files.GetCount() == 1) {
        this->window->open(files[0]);
    }
}
#endif
