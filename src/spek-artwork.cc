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

#include "spek-artwork.h"

class SpekArtProvider : public wxArtProvider
{
protected:
    virtual wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& client, const wxSize& size);
    virtual wxIconBundle CreateIconBundle(const wxArtID& id, const wxArtClient& client);
};

wxBitmap SpekArtProvider::CreateBitmap(
    const wxArtID& id, const wxArtClient& client, const wxSize& size)
{
#ifdef OS_UNIX
    if (id == ART_SPEK) {
        return wxArtProvider::GetBitmap("spek", client, size);
    }
    if (id == ART_HELP) {
        return wxArtProvider::GetBitmap("gtk-help", client, size);
    }
    if (id == ART_OPEN) {
        return wxArtProvider::GetBitmap("gtk-open", client, size);
    }
    if (id == ART_SAVE) {
        return wxArtProvider::GetBitmap("gtk-save", client, size);
    }
    if (id == ART_CLOSE) {
        return wxArtProvider::GetBitmap("gtk-close", client, size);
    }
#endif
#ifdef OS_WIN
    if (id == ART_HELP) {
        return wxIcon("help", wxBITMAP_TYPE_ICO_RESOURCE, 24, 24);
    }
    if (id == ART_OPEN) {
        return wxIcon("open", wxBITMAP_TYPE_ICO_RESOURCE, 24, 24);
    }
    if (id == ART_SAVE) {
        return wxIcon("save", wxBITMAP_TYPE_ICO_RESOURCE, 24, 24);
    }
    if (id == ART_CLOSE) {
        return wxIcon("close", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
    }
#endif
#ifdef OS_OSX
    if (id == ART_HELP) {
        return wxBitmap("help", wxBITMAP_TYPE_PNG_RESOURCE);
    }
    if (id == ART_OPEN) {
        return wxBitmap("open", wxBITMAP_TYPE_PNG_RESOURCE);
    }
    if (id == ART_SAVE) {
        return wxBitmap("save", wxBITMAP_TYPE_PNG_RESOURCE);
    }
    if (id == ART_CLOSE) {
        return wxBitmap("close", wxBITMAP_TYPE_PNG_RESOURCE);
    }
#endif
    return wxNullBitmap;
}

wxIconBundle SpekArtProvider::CreateIconBundle(const wxArtID& id, const wxArtClient& client)
{
#ifdef OS_UNIX
    if (id == ART_SPEK) {
        return wxArtProvider::GetIconBundle("spek", client);
    }
#endif
#ifdef OS_WIN
    if (id == ART_SPEK) {
        wxIconBundle bundle;
        bundle.AddIcon(wxIcon("aaaa", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16));
        bundle.AddIcon(wxIcon("aaaa", wxBITMAP_TYPE_ICO_RESOURCE, 24, 24));
        bundle.AddIcon(wxIcon("aaaa", wxBITMAP_TYPE_ICO_RESOURCE, 32, 32));
        bundle.AddIcon(wxIcon("aaaa", wxBITMAP_TYPE_ICO_RESOURCE, 48, 48));
        return bundle;
    }
#endif
    return wxNullIconBundle;
}

void spek_artwork_init()
{
    wxArtProvider::Push(new SpekArtProvider());
}
