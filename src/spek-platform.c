/* spek-platform.c
 *
 * Copyright (C) 2010  Alexander Kojevnikov <alexander@kojevnikov.com>
 *
 * Spek is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Spek is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Spek.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <glib.h>
#include <gtk/gtk.h>

#ifdef G_OS_WIN32
#include <windows.h>
#include <shellapi.h>
#endif

#include "spek-platform.h"

void spek_platform_fix_args (gchar **argv, gint argc) {
#ifdef G_OS_WIN32
	/* Because MinGW does not support Unicode arguments we are going to
	 * get them using Windows API. In addition, GLib's option parser
	 * doesn't work well with utf-8 strings on Windows, converting
	 * them to URIs works around this problem.
	 */
	int i;
	gchar *s, *t;
	wchar_t **wargv;
	int wargc;
	wargv = CommandLineToArgvW (GetCommandLineW (), &wargc);
	for (i = 0; i < argc; i++) {
		s = g_utf16_to_utf8 (wargv[i], -1, NULL, NULL, NULL);
		if (s) {
			t = g_filename_to_uri (s, NULL, NULL);
			g_free (s);
			if (t) {
				g_free (argv[i]);
				argv[i] = t;
			}
		}
	}
	LocalFree (wargv);
#endif
}

void spek_platform_show_uri (const gchar *uri) {
#ifdef G_OS_WIN32
	/* gtk_show_uri doesn't work on Windows */
	ShellExecuteA (NULL, "open", uri, "", NULL, SW_SHOWNORMAL);
#else
	GError *error = NULL;
	if (!gtk_show_uri (NULL, uri, gtk_get_current_event_time (), &error)) {
		g_error_free (error);
	}
#endif
}
