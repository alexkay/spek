/* spek-platform.cc
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

#include <cstring>

#ifdef OS_OSX
#include <ApplicationServices/ApplicationServices.h>
#endif

#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/utils.h>

#include "spek-platform.h"

void spek_platform_init()
{
#ifdef OS_OSX
    ProcessSerialNumber PSN;
    GetCurrentProcess(&PSN);
    TransformProcessType(&PSN, kProcessTransformToForegroundApplication);
#endif
}

wxString spek_platform_config_path(const wxString& app_name)
{
#ifdef OS_WIN
    wxFileName file_name(wxStandardPaths::Get().GetUserConfigDir(), wxEmptyString);
#else
    wxFileName file_name(wxGetHomeDir(), wxEmptyString);
    file_name.AppendDir(".config");
#endif
    file_name.AppendDir(app_name);
    file_name.Mkdir(0755, wxPATH_MKDIR_FULL);
    file_name.SetFullName("preferences");
    return file_name.GetFullPath();
}

bool spek_platform_can_change_language()
{
#ifdef OS_UNIX
    return false;
#else
    return true;
#endif
}

double spek_platform_font_scale()
{
#ifdef OS_OSX
    return 1.3;
#else
    return 1.0;
#endif
}
