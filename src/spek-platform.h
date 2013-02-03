/* spek-platform.h
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

#ifndef SPEK_PLATFORM_H_
#define SPEK_PLATFORM_H_

#include <wx/string.h>

// Platform-specific initialisation code.
void spek_platform_init();

// Not quite XDG-compatible, but close enough.
wxString spek_platform_config_path(const wxString& app_name);

// Setting non-default locale under GTK+ is tricky (see e.g. how FileZilla does it). We will
// just disable the language setting for GTK+ users and will always use the system locale.
bool spek_platform_can_change_language();

// Fonts are smaller on OSX.
double spek_platform_font_scale();

#endif
