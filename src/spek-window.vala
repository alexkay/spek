using Gdk;
using Gtk;

namespace Spek {
	public class Window : Gtk.Window {

		private Source source;
		private Gtk.Image image;

		public Window () {
			this.title = Config.PACKAGE_STRING;
			this.set_default_size (300, 200);
			this.destroy.connect (Gtk.main_quit);

			var toolbar = new Toolbar ();
			var open = new ToolButton.from_stock (STOCK_OPEN);
			open.clicked.connect (on_open_clicked);
			toolbar.insert (open, -1);
			var quit = new ToolButton.from_stock (STOCK_QUIT);
			quit.clicked.connect (s => this.destroy());
			toolbar.insert (quit, -1);

			image = new Gtk.Image ();
			image.size_allocate.connect (on_image_size_allocate);

			var vbox = new VBox (false, 0);
			vbox.pack_start (toolbar, false, true, 0);
			vbox.pack_start (image, true, true, 0);
			this.add (vbox);
			this.show_all ();
		}

		private void on_open_clicked () {
			var chooser = new FileChooserDialog (
				_ ("Open File"), this, FileChooserAction.OPEN,
				STOCK_CANCEL, ResponseType.CANCEL,
				STOCK_OPEN, ResponseType.ACCEPT, null);
			if (chooser.run () == ResponseType.ACCEPT) {
				var width = image.allocation.width;
				var height = image.allocation.height;
				image.pixbuf = new Pixbuf (Colorspace.RGB, false, 8, width, height);
				// TODO: clear the pixbuf
				source = new Source (chooser.get_filename (), height, width, source_callback);
			}
			chooser.destroy ();
		}

		private void on_image_size_allocate (Rectangle allocation) {
			// TODO: re-create the source
		}

		private void source_callback (int sample, float[] values) {
			var x = sample;
			unowned uchar[] pixels = image.pixbuf.get_pixels ();
			var rowstride = image.pixbuf.rowstride;
			for (int y = 0; y < values.length; y++) {
				var i = (values.length - y - 1) * rowstride + x * 3;
				var level = float.min (1f, Math.log10f (101f + values[y]) / 2f);
				var value = (uchar) (level * 255);
				pixels[i] = value;
				pixels[i + 1] = value;
				pixels[i + 2] = value;
			}
			var allocation = image.allocation;
			image.queue_draw_area (allocation.x + x, allocation.y, 1, allocation.height);
		}
	}
}
