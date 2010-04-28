using Spek;

int main (string[] args) {
	Intl.bindtextdomain (Config.GETTEXT_PACKAGE, Config.LOCALEDIR);
	Intl.bind_textdomain_codeset (Config.GETTEXT_PACKAGE, "UTF-8");
	Intl.textdomain (Config.GETTEXT_PACKAGE);

	Gtk.init (ref args);

	var window = new Spek.Window ();
	window.destroy.connect (Gtk.main_quit);

	Gtk.main ();
	return 0;
}
