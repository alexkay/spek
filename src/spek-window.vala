/* spek-window.vala
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

using Gdk;
using Gtk;

namespace Spek {
    public class Window : Gtk.Window {

        private UIManager ui;
        private InfoBar info_bar;
        private Spectrogram spectrogram;
        private string description;
        private string cur_dir;
        private FileFilter filter_all;
        private FileFilter filter_audio;
        private FileFilter filter_png;


        private const Gtk.TargetEntry[] DEST_TARGET_ENTRIES = {
            { "text/uri-list", 0, 0 }
        };

        public Window (string? file_name) {
            info_bar = new InfoBar.with_buttons (Stock.OK, ResponseType.OK);
            var label = new Label (null);
            label.use_markup = true;
            label.set_markup (_("A new version of Spek is available on <a href=\"http://www.spek-project.org\">www.spek-project.org</a>"));
            label.ellipsize = Pango.EllipsizeMode.END;
            label.xalign = 0f;
            label.activate_link.connect (uri => { Platform.show_uri (uri); return true; });
            label.show();
            var content_area = (Container) info_bar.get_content_area();
            content_area.add(label);
            info_bar.message_type = MessageType.INFO;
            info_bar.response.connect(() => info_bar.hide());

            var vbox = new VBox (false, 0);
            vbox.pack_start (info_bar, false, true, 0);
            add (vbox);
            vbox.show ();

            show ();

            try {
                Thread.create<void*> (check_version, false);
            } catch (ThreadError e) {
            }
        }

        private void on_edit_preferences () {
            var dlg = new PreferencesDialog ();
            dlg.transient_for = this;
            dlg.run ();
        }

        private void * check_version () {
            // Does the user want to check for updates?
            var prefs = Preferences.instance;
            var check = prefs.check_update;
            if (!check) {
                return null;
            }

            // When was the last update?
            var time_val = TimeVal ();
            time_val.get_current_time ();
            Date today = Date ();
            today.set_time_val (time_val);
            int day = prefs.last_update;
            int diff = (int) today.get_julian () - day;
            if (diff < 7) {
                return null;
            }

            // Get the version number.
            var version = Platform.read_line ("http://www.spek-project.org/version");
            if (version == null) {
                return null;
            }

            if (version != null && version > Config.PACKAGE_VERSION) {
                Idle.add (() => { info_bar.show_all (); return false; });
            }

            // Update the preferences.
            prefs.check_update = check;
            prefs.last_update = (int) today.get_julian ();
            prefs.save ();
            return null;
        }
    }
}
