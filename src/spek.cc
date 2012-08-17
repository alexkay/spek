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

#include "spek-audio.h"
#include "spek-platform.hh"
#include "spek-preferences.hh"

#include "spek-window.hh"

class Spek: public wxApp
{
protected:
    virtual bool OnInit();
    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

private:
    wxString path;
};

IMPLEMENT_APP(Spek)

bool Spek::OnInit()
{
    SpekPlatform::init();
    SpekPreferences::get().init();
    spek_audio_init();

    if (!wxApp::OnInit()) {
        return false;
    }

    SpekWindow *window = new SpekWindow(this->path);
    window->Show(true);
    SetTopWindow(window);
    return true;
}

void Spek::OnInitCmdLine(wxCmdLineParser& parser)
{
    wxCmdLineEntryDesc desc[] = {{
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
            wxCMD_LINE_NONE
        }
    };

    parser.SetDesc(desc);
}

bool Spek::OnCmdLineParsed(wxCmdLineParser& parser)
{
    if (!wxApp::OnCmdLineParsed(parser)) {
        return false;
    }

    if (parser.Found(wxT("version"))) {
        // TRANSLATORS: first %s is the package name, second %s is the package version.
        wxPrintf(_("%s version %s\n"), wxT(PACKAGE_NAME), wxT(PACKAGE_VERSION));
        return false;
    }

    this->path = parser.GetParam();

    return true;
}
