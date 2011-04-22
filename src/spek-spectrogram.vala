/* spek-spectrogram.vala
 *
 * Copyright (C) 2010-2011  Alexander Kojevnikov <alexander@kojevnikov.com>
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
using Pango;

namespace Spek {
	class Spectrogram : DrawingArea {

		public string file_name { get; private set; }
		private Pipeline pipeline;
		private string info;
		private const int THRESHOLD = -92;
		private const int NFFT = 2048;
		private const int BANDS = NFFT / 2 + 1;

		private ImageSurface image;
		private ImageSurface palette;

		private const int LPAD = 60;
		private const int TPAD = 60;
		private const int RPAD = 80;
		private const int BPAD = 40;
		private const int GAP = 10;
		private const int RULER = 10;
		private double FONT_SCALE = Platform.get_font_scale ();

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
			this.info = "";

			start ();
		}

		public void save (string file_name) {
			Allocation allocation;
			get_allocation (out allocation);
			var surface = new ImageSurface (Format.RGB24, allocation.width, allocation.height);
			draw (new Cairo.Context (surface));
			surface.write_to_png (file_name);
		}

		private void start () {
			if (pipeline != null) {
				pipeline.stop ();
			}

			// The number of samples is the number of pixels available for the image.
			// The number of bands is fixed, FFT results are very different for
			// different values but we need some consistency.
			Allocation allocation;
			get_allocation (out allocation);
			int samples = allocation.width - LPAD - RPAD;
			if (samples > 0) {
				image = new ImageSurface (Format.RGB24, samples, BANDS);
				pipeline = new Pipeline (file_name, BANDS, samples, THRESHOLD, data_cb);
				pipeline.start ();
				info = pipeline.description;
			} else {
				image = null;
				pipeline = null;
			}

			queue_draw ();
		}

		private int prev_width = -1;
		protected override void size_allocate (Gdk.Rectangle allocation) {
			base.size_allocate (allocation);

			bool width_changed = prev_width != allocation.width;
			prev_width = allocation.width;

			if (file_name != null && width_changed) {
				start ();
			}
		}

		private double log10_threshold = Math.log10 (-THRESHOLD);
		private void data_cb (int sample, float[] values) {
			for (int y = 0; y < BANDS; y++) {
				var level = double.min (
					1.0, Math.log10 (1.0 - THRESHOLD + values[y]) / log10_threshold);
				put_pixel (image, sample, y, get_color (level));
			}
			Idle.add (() => { queue_draw (); return false; });
		}

		protected override bool expose_event (EventExpose event) {
			var window = get_window ();
			var cr = cairo_create (window);

			// Clip to the exposed area.
			cr.rectangle (event.area.x, event.area.y, event.area.width, event.area.height);
			cr.clip ();

			draw (cr);
			return true;
		}

		private void draw (Cairo.Context cr) {
			Allocation allocation;
			get_allocation (out allocation);
			double w = allocation.width;
			double h = allocation.height;
			int text_width, text_height;

			// Clean the background.
			cr.set_source_rgb (0, 0, 0);
			cr.paint ();

			// Spek version
			cr.set_source_rgb (1, 1, 1);
			var layout = cairo_create_layout (cr);
			layout.set_font_description (FontDescription.from_string (
				"Sans " + (9 * FONT_SCALE).to_string ()));
			layout.set_width (RPAD * Pango.SCALE);
			layout.set_text ("dummy", -1);
			layout.get_pixel_size (out text_width, out text_height);
			int line_height = text_height;
			layout.set_font_description (FontDescription.from_string (
				 "Sans Bold " + (10 * FONT_SCALE).to_string ()));
			layout.set_text (Config.PACKAGE_NAME + " ", -1);
			layout.get_pixel_size (out text_width, out text_height);
			cr.move_to (w - RPAD + GAP, TPAD - 2 * GAP - line_height);
			cairo_show_layout_line (cr, layout.get_line (0));
			layout.set_font_description (FontDescription.from_string (
				"Sans " + (9 * FONT_SCALE).to_string ()));
			layout.set_text (Config.PACKAGE_VERSION, -1);
			cr.rel_move_to (text_width, 0);
			cairo_show_layout_line (cr, layout.get_line (0));

			if (image != null) {
				// Draw the spectrogram.
				cr.translate (LPAD, h - BPAD);
				cr.scale (1, -(h - TPAD - BPAD) / image.get_height ());
				cr.set_source_surface (image, 0, 0);
				cr.paint ();
				cr.identity_matrix ();

				// Prepare to draw the rulers.
				cr.set_source_rgb (1, 1, 1);
				cr.set_line_width (1);
				cr.set_antialias (Antialias.NONE);
				layout.set_font_description (FontDescription.from_string (
					"Sans " + (8 * FONT_SCALE).to_string ()));
				layout.set_width (-1);

				// Time ruler.
				var duration_seconds = (int) pipeline.duration;
				var time_ruler = new Ruler (
					Ruler.Position.BOTTOM,
					// TODO: i18n
					"00:00",
					{1, 2, 5, 10, 20, 30, 1*60, 2*60, 5*60, 10*60, 20*60, 30*60},
					duration_seconds,
					1.5,
					unit => (w - LPAD - RPAD) * unit / duration_seconds,
					p => p,
					// TODO: i18n
					unit => "%d:%02d".printf (unit / 60, unit % 60));
				cr.translate (LPAD, h - BPAD);
				time_ruler.draw (cr, layout);
				cr.identity_matrix ();

				// Frequency ruler.
				var freq = pipeline.sample_rate / 2;
				var rate_ruler = new Ruler (
					Ruler.Position.LEFT,
					// TRANSLATORS: keep "00" unchanged, it's used to calc the text width
					_("00 kHz"),
					{1000, 2000, 5000, 10000, 20000},
					freq,
					3.0,
					unit => (h - TPAD - BPAD) * unit / freq,
					p => p,
					unit => _("%d kHz").printf (unit / 1000));
				cr.translate (LPAD, TPAD);
				rate_ruler.draw (cr, layout);
				cr.identity_matrix ();

				// File properties.
				cr.move_to (LPAD, TPAD - GAP);
				layout.set_font_description (FontDescription.from_string (
					"Sans " + (9 * FONT_SCALE).to_string ()));
				layout.set_width ((int) (w - LPAD - RPAD) * Pango.SCALE);
				layout.set_ellipsize (EllipsizeMode.END);
				layout.set_text (info, -1);
				cairo_show_layout_line (cr, layout.get_line (0));
				layout.get_pixel_size (out text_width, out text_height);

				// File name.
				cr.move_to (LPAD, TPAD - 2 * GAP - text_height);
				layout.set_font_description (FontDescription.from_string (
					"Sans Bold " + (10 * FONT_SCALE).to_string ()));
				layout.set_width ((int) (w - LPAD - RPAD) * Pango.SCALE);
				layout.set_ellipsize (EllipsizeMode.START);
				layout.set_text (file_name, -1);
				cairo_show_layout_line (cr, layout.get_line (0));
			}

			// Border around the spectrogram.
			cr.set_source_rgb (1, 1, 1);
			cr.set_line_width (1);
			cr.set_antialias (Antialias.NONE);
			cr.rectangle (LPAD, TPAD, w - LPAD - RPAD, h - TPAD - BPAD);
			cr.stroke ();

			// The palette.
			cr.translate (w - RPAD + GAP, h - BPAD);
			cr.scale (1, -(h - TPAD - BPAD + 1) / palette.get_height ());
			cr.set_source_surface (palette, 0, 0);
			cr.paint ();
			cr.identity_matrix ();

			// Prepare to draw the ruler.
			cr.set_source_rgb (1, 1, 1);
			cr.set_line_width (1);
			cr.set_antialias (Antialias.NONE);
			layout.set_font_description (FontDescription.from_string (
				"Sans " + (8 * FONT_SCALE).to_string ()));
			layout.set_width (-1);

			// Spectral density.
			var density_ruler = new Ruler (
				Ruler.Position.RIGHT,
				// TRANSLATORS: keep "-00" unchanged, it's used to calc the text width
				_("-00 dB"),
				{1, 2, 5, 10, 20, 50},
				-THRESHOLD,
				3.0,
				unit => -(h - TPAD - BPAD) * unit / THRESHOLD,
				p => h - TPAD - BPAD - p,
				unit => _("%d dB").printf (-unit));
			cr.translate (w - RPAD + GAP + RULER, TPAD);
			density_ruler.draw (cr, layout);
			cr.identity_matrix ();
		}

		private void put_pixel (ImageSurface surface, int x, int y, uint32 color) {
			var i = y * surface.get_stride () + x * 4;
			unowned uchar[] data = surface.get_data ();

			// Translate uchar* to uint32* to avoid dealing with endianness.
			uint32 *p = (uint32 *) (&data[i]);
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
