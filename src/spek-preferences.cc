/* spek-preferences.cc
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

#include <wx/string.h>

#include "spek-platform.hh"

#include "spek-preferences.hh"

SpekPreferences& SpekPreferences::get()
{
    static SpekPreferences instance;
    return instance;
}

void SpekPreferences::init()
{
    if (this->locale) {
        delete this->locale;
    }
    this->locale = new wxLocale();

    int lang = wxLANGUAGE_DEFAULT;
    wxString code = this->get_language();
    if (!code.IsEmpty()) {
        const wxLanguageInfo *info = wxLocale::FindLanguageInfo(code);
        if (info) {
            lang = info->Language;
        }
    }
    this->locale->Init(lang);
    this->locale->AddCatalog(wxT(GETTEXT_PACKAGE));
}

SpekPreferences::SpekPreferences() : locale(NULL)
{
    wxString path = SpekPlatform::config_path(wxT("spek"));
    this->config = new wxFileConfig(
        wxEmptyString,
        wxEmptyString,
        path,
        wxEmptyString,
        wxCONFIG_USE_LOCAL_FILE,
        wxConvUTF8
    );
}

bool SpekPreferences::get_check_update()
{
    bool result = true;
    this->config->Read(wxT("/update/check"), &result);
    return result;
}

void SpekPreferences::set_check_update(bool value)
{
    this->config->Write(wxT("/update/check"), value);
    this->config->Flush();
}

long SpekPreferences::get_last_update()
{
    long result = 0;
    this->config->Read(wxT("/update/last"), &result);
    return result;
}

void SpekPreferences::set_last_update(long value)
{
    this->config->Write(wxT("/update/last"), value);
    this->config->Flush();
}

wxString SpekPreferences::get_language()
{
    wxString result(wxT(""));
    this->config->Read(wxT("/general/language"), &result);
    return result;
}

void SpekPreferences::set_language(const wxString& value)
{
    this->config->Write(wxT("/general/language"), value);
    this->config->Flush();
}
