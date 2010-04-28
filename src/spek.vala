public class Main {
	public static int main (string[] args) {
		Intl.bindtextdomain( Config.GETTEXT_PACKAGE, Config.LOCALEDIR );
		Intl.bind_textdomain_codeset( Config.GETTEXT_PACKAGE, "UTF-8" );
		Intl.textdomain( Config.GETTEXT_PACKAGE );

		Gtk.init (ref args);

		var window = new Gtk.Window (Gtk.WindowType.TOPLEVEL);
		window.set_default_size (300, 200);
		window.destroy += Gtk.main_quit;
		window.add (new Gtk.Label ("Hello World!"));
		window.show_all ();

		Gtk.main ();
		return 0;
	}
}

