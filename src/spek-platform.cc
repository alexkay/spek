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

#include <wx/filename.h>

#include "spek-platform.h"

char * spek_platform_short_path(const char *path)
{
#ifdef OS_WIN
    wxFileName file_name(wxString(path, wxConvUTF8));
    return strdup(file_name.GetShortPath().char_str(wxConvFile));
#endif
    return NULL;
}

