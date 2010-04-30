using Gst;
using Gtk;

namespace Spek {
	public class Window : Gtk.Window {

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

			var vbox = new VBox (false, 0);
			vbox.pack_start (toolbar, false, true, 0);
			this.add (vbox);
			this.show_all ();
		}

		private void on_open_clicked () {
			var chooser = new FileChooserDialog (
				_ ("Open File"), this, FileChooserAction.OPEN,
				STOCK_CANCEL, ResponseType.CANCEL,
				STOCK_OPEN, ResponseType.ACCEPT, null);
			if (chooser.run () == ResponseType.ACCEPT) {
				open (chooser.get_filename ());
			}
			chooser.destroy ();
		}

		private void open (string name) {
			var pipeline = new Pipeline ("pipeline");
			var filesrc = ElementFactory.make ("filesrc", "filesrc");
			var decodebin = ElementFactory.make ("decodebin", "decodebin");
			pipeline.add_many (filesrc, decodebin);
			filesrc.link (decodebin);
			filesrc.set ("location", name);

			Signal.connect (decodebin, "new-decoded-pad", (GLib.Callback) on_new_decoded_pad, pipeline);
			pipeline.set_state (State.PAUSED);
			pipeline.get_state (null, null, -1);

			unowned Iterator it = decodebin.iterate_src_pads ();
			void *data;
			while (it.next (out data) == IteratorResult.OK) {
				var pad = (Pad) data;
				var caps = pad.get_caps ();
				var structure = caps.get_structure (0);
				stdout.printf ("structure=%s\n", structure.to_string ());
			}
		}

		private static void on_new_decoded_pad (
			Element decodebin, Pad new_pad, bool last, Pipeline pipeline) {
			var fakesink = ElementFactory.make ("fakesink", null);
			pipeline.add (fakesink);
			var sinkpad = fakesink.get_static_pad ("sink");
			new_pad.link (sinkpad);
			fakesink.set_state (State.PAUSED);
		}
	}
}
