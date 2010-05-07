using Cairo;
using Gdk;
using Gtk;

namespace Spek {
	class Spectrogram : DrawingArea {

		private Source source;
		private string file_name;
		private const int THRESHOLD = -92;

		private ImageSurface image;

		public Spectrogram () {
			show_all ();
		}

		public void open (string file_name) {
			this.file_name = file_name;
			start ();
		}

		private void start () {
			this.image = new ImageSurface (Format.RGB24, allocation.width, allocation.height);
			if (this.source != null) {
				this.source.stop ();
				this.source = null;
			}
			this.source = new Source (
				file_name, allocation.height, allocation.width,
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
			var x = sample;
			var stride = image.get_stride ();
			unowned uchar[] data = image.get_data ();
			for (int y = 0; y < values.length; y++) {
				var i = (values.length - y - 1) * stride + x * 4;
				var level = float.min (
					1f, Math.log10f (1f - THRESHOLD + values[y]) / Math.log10f (-THRESHOLD));
				uint32 color = get_color (level);
				if (sample < 20) {
					// TODO: allocate additional space for this.
					color = get_color (((float) y) / values.length);
				}
				uint32 *p = &data[i];
				*p = color;
			}
			queue_draw_area (sample, 0, 1, allocation.height);
		}

		private override bool expose_event (EventExpose event) {
			var cr = cairo_create (this.window);

			if (image == null) {
				cr.set_source_rgb (0, 0, 0);
			} else {
				cr.set_source_surface (image, 0, 0);
			}
			cr.rectangle (event.area.x, event.area.y, event.area.width, event.area.height);
			cr.fill ();
			return true;
		}

		// Modified version of Dan Bruton's algorithm:
		// http://www.physics.sfasu.edu/astro/color/spectra.html
		private uint32 get_color (float level) {
			level *= 0.6625f;
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
			}

			// Intensity correction.
			float cf = 1.0f;
			if (level >= 0 && level < 0.1f) {
				cf = level / 0.1f;
			}
			cf *= 255f;

			// Pack RGB values into Cairo-happy format.
			uint32 rr = (uint32) (r * cf + 0.5f);
			uint32 gg = (uint32) (g * cf + 0.5f);
			uint32 bb = (uint32) (b * cf + 0.5f);
			return (rr << 16) + (gg << 8) + bb;
		}
	}
}