using Gdk;
using Gtk;

namespace Spek {
	class Spectrogram : Gtk.Image {

		private Source source;

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
				var value = (uchar) (level * 255f);
				pixels[i] = value;
				pixels[i + 1] = value;
				pixels[i + 2] = value;
			}
			queue_draw_area (allocation.x + sample, allocation.y, 1, allocation.height);
		}
	}
}