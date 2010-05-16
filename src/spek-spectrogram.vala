/* spek-spectrogram.vala
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
using Gdk;
using Gtk;

namespace Spek {
	class Spectrogram : DrawingArea {

		public string file_name { get; private set; }
		private Source source;
		private const int THRESHOLD = -92;
		private const int BANDS = 1024;

		private ImageSurface image;
		private ImageSurface palette;
		private const int PADDING = 40;
		private const int GAP = 10;
		private const int RULER = 10;

		public Spectrogram () {
			// Pre-draw the palette.
			palette = new ImageSurface (Format.RGB24, RULER, BANDS);
			for (int y = 0; y < BANDS; y++) {
				var color = get_color (y / (double) BANDS);
				for (int x = 0; x < RULER; x++) {
					put_pixel (palette, x, y, color);
				}
			}
			show_all ();
		}

		public void open (string file_name) {
			this.file_name = file_name;
			start ();
		}

		public void save (string file_name) {
			var surface = new ImageSurface (Format.RGB24, allocation.width, allocation.height);
			draw (new Context (surface));
			surface.write_to_png (file_name);
		}

		private void start () {
			// The number of samples is the number of pixels available for the image.
			// The number of bands is fixed, FFT results are very different for
			// different values but we need some consistency.
			this.image = new ImageSurface (Format.RGB24, allocation.width - 2 * PADDING, BANDS);

			if (this.source != null) {
				this.source.stop ();
			}
			this.source = new Source (
				file_name, image.get_height (), image.get_width (),
				THRESHOLD, source_callback);
			queue_draw ();
		}

		private override void size_allocate (Gdk.Rectangle allocation) {
			base.size_allocate (allocation);

			if (file_name != null) {
				start ();
			}
		}

		private void source_callback (int sample, float[] values) {
			for (int y = 0; y < values.length; y++) {
				var level = double.min (
					1.0, Math.log10 (1.0 - THRESHOLD + values[y]) / Math.log10 (-THRESHOLD));
				put_pixel (image, sample, y, get_color (level));
			}
			queue_draw_area (PADDING + sample, PADDING, 1, allocation.height - 2 * PADDING);
		}

		private override bool expose_event (EventExpose event) {
			var cr = cairo_create (this.window);

			// Clip to the exposed area.
			cr.rectangle (event.area.x, event.area.y, event.area.width, event.area.height);
			cr.clip ();

			draw (cr);
			return true;
		}

		private void draw (Context cr) {
			double w = allocation.width;
			double h = allocation.height;

			// Clean the background.
			cr.set_source_rgb (0, 0, 0);
			cr.paint ();

			// Draw the spectrogram.
			if (image != null) {
				cr.translate (PADDING, h - PADDING);
				cr.scale (1, -(h - 2 * PADDING) / image.get_height ());
				cr.set_source_surface (image, 0, 0);
				cr.paint ();
				cr.identity_matrix ();
			}

			// Border around the spectrogram.
			cr.set_source_rgb (1, 1, 1);
			cr.set_line_width (1);
			cr.set_antialias (Antialias.NONE);
			cr.rectangle (PADDING, PADDING, w - 2 * PADDING, h - 2 * PADDING);
			cr.stroke ();

			// The palette.
			cr.translate (w - PADDING + GAP, h - PADDING);
			cr.scale (1, -(h - 2 * PADDING) / palette.get_height ());
			cr.set_source_surface (palette, 0, 0);
			cr.paint ();
			cr.identity_matrix ();
		}

		private void put_pixel (ImageSurface surface, int x, int y, uint32 color) {
			var i = y * surface.get_stride () + x * 4;
			unowned uchar[] data = surface.get_data ();

			// Translate uchar* to uint32* to avoid dealing with endianness.
			uint32 *p = &data[i];
			*p = color;
		}

		// Modified version of Dan Bruton's algorithm:
		// http://www.physics.sfasu.edu/astro/color/spectra.html
		private uint32 get_color (double level) {
			level *= 0.6625;
			double r = 0.0, g = 0.0, b = 0.0;
			if (level >= 0 && level < 0.15) {
				r = (0.15 - level) / (0.15 + 0.075);
				g = 0.0;
				b = 1.0;
			} else if (level >= 0.15 && level < 0.275) {
				r = 0.0;
				g = (level - 0.15) / (0.275 - 0.15);
				b = 1.0;
			} else if (level >= 0.275 && level < 0.325) {
				r = 0.0;
				g = 1.0;
				b = (0.325 - level) / (0.325 - 0.275);
			} else if (level >= 0.325 && level < 0.5) {
				r = (level - 0.325) / (0.5 - 0.325);
				g = 1.0;
				b = 0.0;
			} else if (level >= 0.5 && level < 0.6625) {
				r = 1.0;
				g = (0.6625 - level) / (0.6625 - 0.5f);
				b = 0.0;
			}

			// Intensity correction.
			double cf = 1.0;
			if (level >= 0.0 && level < 0.1) {
				cf = level / 0.1;
			}
			cf *= 255.0;

			// Pack RGB values into Cairo-happy format.
			uint32 rr = (uint32) (r * cf + 0.5);
			uint32 gg = (uint32) (g * cf + 0.5);
			uint32 bb = (uint32) (b * cf + 0.5);
			return (rr << 16) + (gg << 8) + bb;
		}
	}
}