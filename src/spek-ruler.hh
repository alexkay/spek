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

    SpekRuler(Position pos, wxString sample_label, int *factors, int units, double spacing);

    void draw(wxDC& dc);

protected:
    virtual double measure(int unit) = 0;
    virtual double place(double p) = 0;
    virtual wxString format(int unit) = 0;

    void draw_tick(wxDC& dc, int tick);

    Position pos;
    wxString sample_label;
    int *factors;
    int units;
    double spacing;
};

#endif
