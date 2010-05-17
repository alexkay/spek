/* spek-ruler.vala
 *
 * Copyright (C) 2010  Alexander Kojevnikov <alexander@kojevnikov.com>
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

namespace Spek {
	class Ruler : GLib.Object {

		private string sample_label;
		private int[] factors;
		private int units;
		private double spacing;
		private UnitToPixel unit_to_pixel;
		private FormatTick format_tick;

		public delegate double UnitToPixel (int unit);
		public delegate string FormatTick (int unit);

		public Ruler (
			string sample_label, int[] factors, int units, double spacing,
			UnitToPixel unit_to_pixel, FormatTick format_tick) {
			this.sample_label = sample_label;
			this.factors = factors;
			this.units = units;
			this.spacing = spacing;
			this.unit_to_pixel = unit_to_pixel;
			this.format_tick = format_tick;
		}

		public void draw (Context cr, bool horizontal) {
			// Mesure the sample label.
			TextExtents ext;
			cr.text_extents (sample_label, out ext);
			var size = horizontal ? ext.width : ext.height;

			// Select the factor to use, we want some space between the labels.
			int factor = 0;
			foreach (var f in factors) {
				if (unit_to_pixel (f) >= spacing * size) {
					factor = f;
					break;
				}
			}

			// Add the ticks.
			int[] ticks = { 0, units };
			if (factor > 0) {
				for (var tick = factor; tick < units; tick += factor) {
					if (unit_to_pixel (units - tick) < size * 1.2) {
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
				var pos = unit_to_pixel (horizontal ? tick : units - tick);
				cr.text_extents (label, out ext);
				// TODO: use font measurements instead ext.height
				if (horizontal) {
					cr.move_to (pos - ext.width / 2, GAP + ext.height);
				} else {
					cr.move_to (-ext.width - GAP, pos + ext.height / 2);
				}
				cr.show_text (label);
				if (horizontal) {
					cr.move_to (pos, 0);
					cr.rel_line_to (0, TICK_LEN);
				} else {
					cr.move_to (0, pos);
					cr.rel_line_to (-TICK_LEN, 0);
				}
				cr.stroke ();
			}
		}
	}
}