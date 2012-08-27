/* spek-preferences-dialog.hh
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

#ifndef SPEK_PREFERENCES_DIALOG_HH_
#define SPEK_PREFERENCES_DIALOG_HH_

#include <wx/wx.h>

class SpekPreferencesDialog : public wxDialog
{
public:
    SpekPreferencesDialog(wxWindow *parent);

private:
    void on_language(wxCommandEvent& event);
    void on_check(wxCommandEvent& event);

    wxArrayString languages;

    DECLARE_EVENT_TABLE()
};

#endif
