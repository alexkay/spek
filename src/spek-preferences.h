/* spek-preferences.h
 *
 * Copyright (C) 2011-2012  Alexander Kojevnikov <alexander@kojevnikov.com>
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

#ifndef SPEK_PREFERENCES_H_
#define SPEK_PREFERENCES_H_

#include <wx/fileconf.h>
#include <wx/intl.h>

class SpekPreferences
{
public:
    static SpekPreferences& get();

    void init();
    bool get_check_update();
    void set_check_update(bool value);
    long get_last_update();
    void set_last_update(long value);
    wxString get_language();
    void set_language(const wxString& value);

private:
    SpekPreferences();
    SpekPreferences(const SpekPreferences&);
    void operator=(const SpekPreferences&);

    wxLocale *locale;
    wxFileConfig *config;
};

#endif
