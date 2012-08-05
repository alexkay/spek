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

        private const Gtk.ActionEntry[] ACTION_ENTRIES = {
            { "File", null, N_("_File") },
            { "FileOpen", Stock.OPEN, null, null, null, on_file_open },
            { "FileSave", Stock.SAVE, null, null, null, on_file_save },
            { "FileQuit", Stock.QUIT, null, null, null, on_file_quit },
            { "Edit", null, N_("_Edit") },
            { "EditPreferences", Stock.PREFERENCES, null, "<Ctrl>E", null, on_edit_preferences },
            { "Help", null, N_("_Help") },
            { "HelpAbout", Stock.ABOUT, null, "F1", null, on_help_about }
        };

        private const string UI = """
<ui>
    <menubar name='MenuBar'>
        <menu action='File'>
            <menuitem action='FileOpen'/>
            <menuitem action='FileSave'/>
            <separator/>
            <menuitem action='FileQuit'/>
        </menu>
        <menu action='Edit'>
            <menuitem action='EditPreferences'/>
        </menu>
        <menu action='Help'>
            <menuitem action='HelpAbout'/>
        </menu>
    </menubar>

    <toolbar name='ToolBar'>
        <toolitem action='FileOpen'/>
        <toolitem action='FileSave'/>
        <separator expand='true'/>
        <toolitem action='HelpAbout'/>
    </toolbar>
</ui>
""";

        private const Gtk.TargetEntry[] DEST_TARGET_ENTRIES = {
            { "text/uri-list", 0, 0 }
        };

        public Window (string? file_name) {
            description = title = _("Spek - Acoustic Spectrum Analyser");
            set_default_icon_name ("spek");
            set_default_size (640, 480);
            destroy.connect (Gtk.main_quit);

            var actions = new Gtk.ActionGroup ("Actions");
            actions.set_translation_domain (Config.GETTEXT_PACKAGE);
            actions.add_actions (ACTION_ENTRIES, this);
            ui = new UIManager ();
            ui.insert_action_group (actions, 0);
            add_accel_group (ui.get_accel_group ());
            try {
                ui.add_ui_from_string (UI, -1);
            } catch (Error e) {
                warning ("Could not load the UI: %s\n", e.message);
            }

            var menubar = ui.get_widget ("/MenuBar");
            var toolbar = (Toolbar) ui.get_widget ("/ToolBar");
            toolbar.set_style (ToolbarStyle.BOTH_HORIZ);
            ((ToolItem) ui.get_widget ("/ToolBar/FileOpen")).is_important = true;
            ((ToolItem) ui.get_widget ("/ToolBar/FileSave")).is_important = true;
            ((ToolItem) ui.get_widget ("/ToolBar/HelpAbout")).is_important = true;

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

            spectrogram = new Spectrogram ();
            cur_dir = Environment.get_home_dir ();

            filter_all = new FileFilter ();
            filter_all.set_name (_("All files"));
            filter_all.add_pattern ("*");
            filter_png = new FileFilter ();
            filter_png.set_name (_("PNG images"));
            filter_png.add_pattern ("*.png");
            filter_audio = new FileFilter ();
            filter_audio.set_name (_("Audio files"));
            foreach (var ext in audio_extensions) {
                filter_audio.add_pattern (ext);
            }

            var vbox = new VBox (false, 0);
            vbox.pack_start (menubar, false, true, 0);
            vbox.pack_start (toolbar, false, true, 0);
            vbox.pack_start (info_bar, false, true, 0);
            vbox.pack_start (spectrogram, true, true, 0);
            add (vbox);
            menubar.show_all ();
            toolbar.show_all ();
            spectrogram.show_all ();
            vbox.show ();

            Platform.fix_ui (ui);
            show ();

            // Set up Drag and Drop
            drag_dest_set (this, DestDefaults.ALL, DEST_TARGET_ENTRIES, DragAction.COPY);
            drag_data_received.connect (on_dropped);

            if (file_name != null) {
                open_file (file_name);
            }

            try {
                Thread.create<void*> (check_version, false);
            } catch (ThreadError e) {
            }
        }

        void on_dropped (DragContext cx, int x, int y, SelectionData data, uint info, uint time) {
            if (data.get_length () > 0 && data.get_format () == 8) {
                string[] files = data.get_uris ();
                if (files.length > 0) {
                    try {
                        open_file (Filename.from_uri (files[0]));
                        drag_finish (cx, true, false, time);
                        return;
                    } catch (ConvertError e) {}
                }
            }
            drag_finish (cx, false, false, time);
        }

        private void open_file (string file_name) {
            cur_dir = Path.get_dirname (file_name);
            spectrogram.open (file_name);

            // TRANSLATORS: window title, %s is replaced with the file name
            title = _("Spek - %s").printf (Path.get_basename (file_name));
        }

        private void on_file_open () {
            var chooser = new FileChooserDialog (
                _("Open File"), this, FileChooserAction.OPEN,
                Stock.CANCEL, ResponseType.CANCEL,
                Stock.OPEN, ResponseType.ACCEPT, null);
            chooser.set_default_response (ResponseType.ACCEPT);
            chooser.select_multiple = false;
            chooser.set_current_folder (cur_dir);
            chooser.add_filter (filter_all);
            chooser.add_filter (filter_audio);
            chooser.set_filter (filter_audio);
            if (chooser.run () == ResponseType.ACCEPT) {
                open_file (chooser.get_filename ());
            }
            chooser.destroy ();
        }

        private void on_file_save () {
            var chooser = new FileChooserDialog (
                _("Save Spectrogram"), this, FileChooserAction.SAVE,
                Stock.CANCEL, ResponseType.CANCEL,
                Stock.SAVE, ResponseType.ACCEPT, null);
            chooser.set_default_response (ResponseType.ACCEPT);
            chooser.set_current_folder (cur_dir);

            // Suggested name is <file_name>.png
            var file_name = Path.get_basename (spectrogram.file_name ?? _("Untitled"));
            file_name += ".png";
            chooser.set_current_name (file_name);
            chooser.add_filter (filter_png);
            chooser.set_filter (filter_png);
            if (chooser.run () == ResponseType.ACCEPT) {
                file_name = chooser.get_filename ();
                cur_dir = Path.get_dirname (file_name);
                spectrogram.save (file_name);
            }
            chooser.destroy ();
        }

        private void on_file_quit () {
            destroy ();
        }

        private void on_edit_preferences () {
            var dlg = new PreferencesDialog ();
            dlg.transient_for = this;
            dlg.run ();
        }

        private void on_help_about () {
            string[] authors = {
                "Primary Development:",
                "\tAlexander Kojevnikov (maintainer)",
                "",
                "Contributors:",
                "\tFabian Deutsch",
                "\tJonathan Gonzalez V",
                "\tStefan Kost",
                "\tThibault North"
            };
            string[] artists = {
                "Olga Vasylevska"
            };
            string license = "Copyright (C) 2010-2011  Alexander Kojevnikov";
            license += "\n\n";
            license += "Spek is free software: you can redistribute it and/or modify ";
            license += "it under the terms of the GNU General Public License as published by ";
            license += "the Free Software Foundation, either version 3 of the License, or ";
            license += "(at your option) any later version.";
            license += "\n\n";
            license += "Spek is distributed in the hope that it will be useful, ";
            license += "but WITHOUT ANY WARRANTY; without even the implied warranty of ";
            license += "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the ";
            license += "GNU General Public License for more details.";
            license += "\n\n";
            license += "You should have received a copy of the GNU General Public License ";
            license += "along with Spek.  If not, see http://www.gnu.org/licenses/";

            var dlg = new AboutDialog ();
            dlg.program_name = "Spek";
            dlg.version = Config.PACKAGE_VERSION;
            dlg.copyright = _("Copyright \xc2\xa9 2010-2011 Alexander Kojevnikov");
            dlg.comments = description;
            dlg.set ("authors", authors);
//          dlg.set ("documenters", documenters);
            dlg.set ("artists", artists);
            dlg.website_label = _("Spek Website");
            dlg.website = "http://www.spek-project.org/";
            dlg.license = license;
            dlg.wrap_license = true;
            try {
                dlg.logo = IconTheme.get_default ().load_icon ("spek", 128, IconLookupFlags.FORCE_SVG);
            } catch (Error e) {
                dlg.logo_icon_name = "spek";
            }
            // TRANSLATORS: Add your name here
            dlg.translator_credits = _("translator-credits");
            dlg.set_transient_for (this);
            dlg.destroy_with_parent = true;
            dlg.response.connect (id => dlg.destroy ());
            dlg.set_url_hook (url_hook);
            dlg.modal = true;
            dlg.present ();
        }

        private void url_hook (AboutDialog about, string link) {
            Platform.show_uri (link);
        }

        // TODO: s/audio/media/
        private string[] audio_extensions = {
            "*.3gp",
            "*.aac",
            "*.aif",
            "*.aifc",
            "*.aiff",
            "*.amr",
            "*.awb",
            "*.ape",
            "*.au",
            "*.dts",
            "*.flac",
            "*.gsm",
            "*.m4a",
            "*.m4p",
            "*.mp3",
            "*.mp4",
            "*.mp+",
            "*.mpc",
            "*.mpp",
            "*.oga",
            "*.ogg",
            "*.ra",
            "*.ram",
            "*.snd",
            "*.wav",
            "*.wma",
            "*.wv"
        };

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
