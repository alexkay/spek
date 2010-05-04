using Gtk;

namespace Spek {
	public class Window : Gtk.Window {

		private Source source;

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
				source = new Source (chooser.get_filename (), 10, 100);
			}
			chooser.destroy ();
		}
	}
}
