/* spek-ruler.cc
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

#include "spek-ruler.hh"

SpekRuler::SpekRuler(Position pos, wxString sample_label, int *factors, int units, double spacing) :
    pos(pos), sample_label(sample_label), factors(factors), units(units), spacing(spacing)
{
}

void SpekRuler::draw(wxDC& dc)
{
    // Mesure the sample label.
    wxSize size = dc.GetTextExtent(sample_label);
    int len = this->pos == TOP || this->pos == BOTTOM ? size.GetWidth() : size.GetHeight();

    // Select the factor to use, we want some space between the labels.
    int factor = 0;
    for (int i = 0; factors[i]; ++i) {
        if (this->measure(factors[i]) >= this->spacing * len) {
            factor = factors[i];
            break;
        }
    }

    // Draw the ticks.
    this->draw_tick(dc, 0);
    this->draw_tick(dc, units);

    if (factor > 0) {
        for (int tick = factor; tick < units; tick += factor) {
            if (this->measure(units - tick) < len * 1.2) {
                break;
            }
            this->draw_tick(dc, tick);
        }
    }
}

void SpekRuler::draw_tick(wxDC& dc, int tick)
{
    double GAP = 10;
    double TICK_LEN = 4;

    wxString label = format(tick);
    double p = place(measure(this->pos == TOP || this->pos == BOTTOM ? tick : this->units - tick));
    wxSize size = dc.GetTextExtent(label);
    int w = size.GetWidth();
    int h = size.GetHeight();

    if (this->pos == TOP) {
        dc.DrawText(label, p - w / 2, -GAP - h);
    } else if (this->pos == RIGHT){
        dc.DrawText(label, GAP, p + h / 4);
    } else if (this->pos == BOTTOM) {
        dc.DrawText(label, p - w / 2, GAP + h);
    } else if (this->pos == LEFT){
        dc.DrawText(label, -w - GAP, p + h / 4);
    }

    if (this->pos == TOP) {
        dc.DrawLine(p, 0, p, -TICK_LEN);
    } else if (this->pos == RIGHT) {
        dc.DrawLine(0, p, TICK_LEN, p);
    } else if (this->pos == BOTTOM) {
        dc.DrawLine(p, 0, p, TICK_LEN);
    } else if (this->pos == LEFT) {
        dc.DrawLine(0, p, -TICK_LEN, p);
    }
}
