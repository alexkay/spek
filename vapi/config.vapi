[CCode (prefix = "", lower_case_cprefix = "", cheader_filename = "config.h")]
namespace Config {
	/* Package information */
	public const string PACKAGE_NAME;
	public const string PACKAGE_STRING;
	public const string PACKAGE_VERSION;

	/* Gettext package */
	public const string GETTEXT_PACKAGE;

	/* Configured paths - these variables are not present in config.h, they are
	 * passed to underlying C code as cmd line macros. */
	public const string LOCALEDIR;  /* /usr/local/share/locale  */
	public const string PKGDATADIR; /* /usr/local/share/spek */
	public const string PKGLIBDIR;  /* /usr/local/lib/spek   */
}

// TODO: file a bug to have this included
[CCode (cprefix = "G", lower_case_cprefix = "g_", cheader_filename = "glib.h", gir_namespace = "GLib", gir_version = "2.0")]
namespace GLib {
	public static string filename_from_uri (string uri, out string hostname) throws Error;
}
