using Spek;

int main (string[] args) {
	Intl.bindtextdomain (Config.GETTEXT_PACKAGE, Config.LOCALEDIR);
	Intl.bind_textdomain_codeset (Config.GETTEXT_PACKAGE, "UTF-8");
	Intl.textdomain (Config.GETTEXT_PACKAGE);

	Gtk.init (ref args);
	Gst.init (ref args);
	var window = new Spek.Window ();
	Gtk.main ();
	window.destroy ();
	return 0;
}
