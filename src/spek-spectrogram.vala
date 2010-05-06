using Gdk;
using Gtk;

namespace Spek {
	class Spectrogram : Gtk.Image {

		private Source source;

		private struct Color { uchar r; uchar g; uchar b; }

		public Spectrogram () {
		}

		public void show_file (string file_name) {
			pixbuf = new Pixbuf (Colorspace.RGB, false, 8, allocation.width, allocation.height);
			pixbuf.fill (0);
			source = new Source (file_name, allocation.height, allocation.width, source_callback);
		}

		private void source_callback (int sample, float[] values) {
			var x = sample;
			var rowstride = pixbuf.rowstride;
			unowned uchar[] pixels = pixbuf.get_pixels ();
			for (int y = 0; y < values.length; y++) {
				var i = (values.length - y - 1) * rowstride + x * 3;
				var level = float.min (1f, Math.log10f (101f + values[y]) / 2f);
				var color = get_color (level);
				if (sample < 20) {
					// TODO: allocate additional space for this.
					color = get_color (((float) y) / values.length);
				}
				pixels[i] = color.r;
				pixels[i + 1] = color.g;
				pixels[i + 2] = color.b;
			}
			queue_draw_area (allocation.x + sample, allocation.y, 1, allocation.height);
		}

		// Modified version of Dan Bruton's algorithm:
		// http://www.physics.sfasu.edu/astro/color/spectra.html
		private Color get_color (float level) {
			float r = 0.0f, g = 0.0f, b = 0.0f;
			if (level >= 0f && level < 0.15f) {
				r = (0.15f - level) / (0.15f + 0.075f);
				g = 0.0f;
				b = 1.0f;
			} else if (level >= 0.15f && level < 0.275f) {
				r = 0.0f;
				g = (level - 0.15f) / (0.275f - 0.15f);
				b = 1.0f;
			} else if (level >= 0.275f && level < 0.325f) {
				r = 0.0f;
				g = 1.0f;
				b = (0.325f - level) / (0.325f - 0.275f);
			} else if (level >= 0.325f && level < 0.5f) {
				r = (level - 0.325f) / (0.5f - 0.325f);
				g = 1.0f;
				b = 0.0f;
			} else if (level >= 0.5f && level < 0.6625f) {
				r = 1.0f;
				g = (0.6625f - level) / (0.6625f - 0.5f);
				b = 0.0f;
			} else if (level >= 0.6625 && level <= 1.0f) {
				r = 1.0f;
				g = 0.0f;
				b = 0.0f;
			}

			// Intensity correction.
			float cf = 0.0f;
			if (level >= 0 && level < 0.1f) {
				cf = 0.3f + 0.7f * (level + 0.075f) / (0.1f + 0.075f);
			} else if (level >= 0.1f && level <= 0.8f) {
				cf = 1.0f;
			} else if (level > 0.8f && level <= 1.0f) {
				cf = 0.3f + 0.7f * (1.0f - level) / (1.0f - 0.8f);
			}
			cf *= 255f;
			return { (uchar) (r * cf + 0.5f), (uchar) (g * cf + 0.5f), (uchar) (b * cf + 0.5f) };
		}
	}
}