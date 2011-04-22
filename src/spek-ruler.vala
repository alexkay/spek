/* spek-ruler.vala
 *
 * Copyright (C) 2010,2011  Alexander Kojevnikov <alexander@kojevnikov.com>
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

using Cairo;
using Pango;

namespace Spek {
	class Ruler : GLib.Object {
		public enum Position {
			TOP,
			RIGHT,
			BOTTOM,
			LEFT
		}

		private Position pos;
		private string sample_label;
		private int[] factors;
		private int units;
		private double spacing;
		private Measure measure;
		private Place place;
		private FormatTick format_tick;

		public delegate double Measure (int unit);
		public delegate double Place (double p);
		public delegate string FormatTick (int unit);

		public Ruler (
			Position pos, string sample_label,
			int[] factors, int units, double spacing,
			Measure measure, Place place, FormatTick format_tick) {
			this.pos = pos;
			this.sample_label = sample_label;
			this.factors = factors;
			this.units = units;
			this.spacing = spacing;
			this.measure = measure;
			this.place = place;
			this.format_tick = format_tick;
		}

		public void draw (Cairo.Context cr, Pango.Layout layout) {
			// Mesure the sample label.
			int w, h;
			layout.set_text (sample_label, -1);
			layout.get_pixel_size (out w, out h);
			var size = pos == Position.TOP || pos == Position.BOTTOM ? w : h;

			// Select the factor to use, we want some space between the labels.
			int factor = 0;
			foreach (var f in factors) {
				if (measure (f) >= spacing * size) {
					factor = f;
					break;
				}
			}

			// Add the ticks.
			int[] ticks = { 0, units };
			if (factor > 0) {
				for (var tick = factor; tick < units; tick += factor) {
					if (measure (units - tick) < size * 1.2) {
						break;
					}
					ticks += tick;
				}
				// TODO: `ticks = ticks[0:-1]` crashes, file a bug.
			}

			// Draw the ticks.
			double GAP = 10;
			double TICK_LEN = 4;
			foreach (var tick in ticks) {
				var label = format_tick (tick);
				var p = place (measure (
					pos == Position.TOP || pos == Position.BOTTOM
					? tick : units - tick));
				layout.set_text (label, -1);
				layout.get_pixel_size (out w, out h);
				if (pos == Position.TOP) {
					cr.move_to (p - w / 2, -GAP - h);
				} else if (pos == Position.RIGHT){
					cr.move_to (GAP, p + h / 4);
				} else if (pos == Position.BOTTOM) {
					cr.move_to (p - w / 2, GAP + h);
				} else if (pos == Position.LEFT){
					cr.move_to (-w - GAP, p + h / 4);
				}
				cairo_show_layout_line (cr, layout.get_line (0));
				if (pos == Position.TOP) {
					cr.move_to (p, 0);
					cr.rel_line_to (0, -TICK_LEN);
				} else if (pos == Position.RIGHT) {
					cr.move_to (0, p);
					cr.rel_line_to (TICK_LEN, 0);
				} else if (pos == Position.BOTTOM) {
					cr.move_to (p, 0);
					cr.rel_line_to (0, TICK_LEN);
				} else if (pos == Position.LEFT) {
					cr.move_to (0, p);
					cr.rel_line_to (-TICK_LEN, 0);
				}
				cr.stroke ();
			}
		}
	}
}