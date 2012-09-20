/* spek-ruler.hh
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

#ifndef SPEK_RULER_HH_
#define SPEK_RULER_HH_

#include <wx/dc.h>
#include <wx/string.h>

class SpekRuler
{
public:
    enum Position
    {
        TOP,
        RIGHT,
        BOTTOM,
        LEFT
    };

    typedef wxString (*formatter_cb)(int unit);

    SpekRuler(
        int x, int y, Position pos, wxString sample_label,
        int *factors, int min_units, int max_units, double spacing,
        double scale, double offset, formatter_cb formatter
    );

    void draw(wxDC& dc);

protected:
    void draw_tick(wxDC& dc, int tick);

    int x;
    int y;
    Position pos;
    wxString sample_label;
    int *factors;
    int min_units;
    int max_units;
    double spacing;
    double scale;
    double offset;
    formatter_cb formatter;
};

#endif
