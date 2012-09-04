/* spek-artwork.cc
 *
 * Copyright (C) 2012  Alexander Kojevnikov <alexander@kojevnikov.com>
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
#include <wx/iconbndl.h>

#include "spek-artwork.hh"

class SpekArtProvider : public wxArtProvider
{
protected:
    virtual wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& client, const wxSize& size);
#if ART_HAS_ICON_BUNDLES
    virtual wxIconBundle CreateIconBundle(const wxArtID& id, const wxArtClient& client);
#endif
};

wxBitmap SpekArtProvider::CreateBitmap(
    const wxArtID& id, const wxArtClient& client, const wxSize& size)
{
    if (id == ART_SPEK) {
#ifdef OS_UNIX
        return wxArtProvider::GetBitmap(wxT("spek"), client, size);
#endif
    }
    return wxNullBitmap;
}

#if ART_HAS_ICON_BUNDLES
wxIconBundle SpekArtProvider::CreateIconBundle(const wxArtID& id, const wxArtClient& client)
{
    if (id == ART_SPEK) {
#ifdef OS_UNIX
        return wxArtProvider::GetIconBundle(wxT("spek"), client);
#endif
#ifdef OS_WIN
        wxIconBundle bundle;
        bundle.AddIcon(wxIcon(wxT("aaaa"), wxBITMAP_TYPE_ICO_RESOURCE, 16, 16));
        bundle.AddIcon(wxIcon(wxT("aaaa"), wxBITMAP_TYPE_ICO_RESOURCE, 24, 24));
        bundle.AddIcon(wxIcon(wxT("aaaa"), wxBITMAP_TYPE_ICO_RESOURCE, 32, 32));
        bundle.AddIcon(wxIcon(wxT("aaaa"), wxBITMAP_TYPE_ICO_RESOURCE, 48, 48));
        return bundle;
#endif
    }
    return wxNullIconBundle;
}
#endif

void spek_artwork_init()
{
    wxArtProvider::Push(new SpekArtProvider());
}
