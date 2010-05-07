using Gdk;
using Gtk;

namespace Spek {
	public class Window : Gtk.Window {

		private Spectrogram spectrogram;

		public Window () {
			this.title = Config.PACKAGE_STRING;
			this.set_default_size (640, 480);
			this.destroy.connect (Gtk.main_quit);

			var group = new AccelGroup ();
			this.add_accel_group (group);

			var toolbar = new Toolbar ();
			toolbar.set_style (ToolbarStyle.BOTH_HORIZ);

			var open = new ToolButton.from_stock (STOCK_OPEN);
			open.is_important = true;
			open.add_accelerator (
				"clicked", group, 'O', ModifierType.CONTROL_MASK, AccelFlags.VISIBLE);
			open.clicked.connect (on_open_clicked);
			toolbar.insert (open, -1);

			toolbar.insert (new SeparatorToolItem (), -1);

			var quit = new ToolButton.from_stock (STOCK_QUIT);
			quit.is_important = true;
			quit.add_accelerator (
				"clicked", group, 'Q', ModifierType.CONTROL_MASK, AccelFlags.VISIBLE);
			quit.clicked.connect (s => this.destroy());
			toolbar.insert (quit, -1);

			this.spectrogram = new Spectrogram ();

			var vbox = new VBox (false, 0);
			vbox.pack_start (toolbar, false, true, 0);
			vbox.pack_start (spectrogram, true, true, 0);
			this.add (vbox);
			this.show_all ();
		}

		private void on_open_clicked () {
			var chooser = new FileChooserDialog (
				_ ("Open File"), this, FileChooserAction.OPEN,
				STOCK_CANCEL, ResponseType.CANCEL,
				STOCK_OPEN, ResponseType.ACCEPT, null);
			if (chooser.run () == ResponseType.ACCEPT) {
				spectrogram.open (chooser.get_filename ());
			}
			chooser.destroy ();
		}
	}
}
