/* spek.vala
 *
 * Copyright (C) 2010-2011  Alexander Kojevnikov <alexander@kojevnikov.com>
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

namespace Spek {
    bool version = false;
    [CCode (array_length = false, array_null_terminated = true)]
    string[] files = null;

    int main (string[] args) {
        if (Preferences.instance.language.length > 0) {
            Environment.set_variable ("LANGUAGE", Preferences.instance.language, true);
        }

        Intl.bindtextdomain (Config.GETTEXT_PACKAGE, Platform.locale_dir ());
        Intl.bind_textdomain_codeset (Config.GETTEXT_PACKAGE, "UTF-8");
        Intl.textdomain (Config.GETTEXT_PACKAGE);

        Audio.init ();
        var file_name = files == null ? null : files[0];
        if (file_name != null && file_name.has_prefix ("file://")) {
            try {
                file_name = Filename.from_uri (file_name);
            } catch (ConvertError e) {
            }
        }
        return 0;
    }
}