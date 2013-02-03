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

#include <cmath>

#include "spek-ruler.h"

SpekRuler::SpekRuler(
    int x, int y, Position pos, wxString sample_label,
    int *factors, int min_units, int max_units, double spacing,
    double scale, double offset, formatter_cb formatter)
    :
    x(x), y(y), pos(pos), sample_label(sample_label),
    factors(factors), min_units(min_units), max_units(max_units), spacing(spacing),
    scale(scale), offset(offset), formatter(formatter)
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
        if (fabs(this->scale * factors[i]) >= this->spacing * len) {
            factor = factors[i];
            break;
        }
    }

    // Draw the ticks.
    this->draw_tick(dc, min_units);
    this->draw_tick(dc, max_units);

    if (factor > 0) {
        for (int tick = min_units + factor; tick < max_units; tick += factor) {
            if (fabs(this->scale * (max_units - tick)) < len * 1.2) {
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

    wxString label = this->formatter(tick);
    int value = this->pos == TOP || this->pos == BOTTOM ?
        tick : this->max_units + this->min_units - tick;
    double p = this->offset + this->scale * (value - min_units);
    wxSize size = dc.GetTextExtent(label);
    int w = size.GetWidth();
    int h = size.GetHeight();

    if (this->pos == TOP) {
        dc.DrawText(label, this->x + p - w / 2, this->y - GAP - h);
    } else if (this->pos == RIGHT){
        dc.DrawText(label, this->x + GAP, this->y + p - h / 2);
    } else if (this->pos == BOTTOM) {
        dc.DrawText(label, this->x + p - w / 2, this->y + GAP);
    } else if (this->pos == LEFT){
        dc.DrawText(label, this->x - w - GAP, this->y + p - h / 2);
    }

    if (this->pos == TOP) {
        dc.DrawLine(this->x + p, this->y, this->x + p, this->y - TICK_LEN);
    } else if (this->pos == RIGHT) {
        dc.DrawLine(this->x, this->y + p, this->x + TICK_LEN, this->y + p);
    } else if (this->pos == BOTTOM) {
        dc.DrawLine(this->x + p, this->y, this->x + p, this->y + TICK_LEN);
    } else if (this->pos == LEFT) {
        dc.DrawLine(this->x, this->y + p, this->x - TICK_LEN, this->y + p);
    }
}
