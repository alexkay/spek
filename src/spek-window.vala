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
		private MessageBar message_bar;
		private Spectrogram spectrogram;
		private string description;
		private string cur_dir;
		private FileFilter filter_all;
		private FileFilter filter_audio;
		private FileFilter filter_png;

		private const ActionEntry[] ACTION_ENTRIES = {
			{ "OpenAction", STOCK_OPEN, null, null, null, on_open_action },
			{ "SaveAction", STOCK_SAVE, null, null, null, on_save_action },
			{ "PreferencesAction", STOCK_PREFERENCES, null, "<Ctrl>E", null, on_prefs_action },
			{ "QuitAction", STOCK_QUIT, null, null, null, on_quit_action },
			{ "AboutAction", STOCK_ABOUT, null, "F1", null, on_about_action }
		};

		private const Gtk.TargetEntry[] DEST_TARGET_ENTRIES = {
			{ "text/uri-list", 0, 0 }
		};

		public Window (string? file_name) {
			description = title = _("Spek - Acoustic Spectrum Analyser");
			set_default_icon_name ("spek");
			set_default_size (640, 480);
			destroy.connect (Gtk.main_quit);

			ui = new UIManager ();
			var actions = new Gtk.ActionGroup ("Actions");
			actions.add_actions (ACTION_ENTRIES, this);

			var toolbar = new Toolbar ();
			toolbar.set_style (ToolbarStyle.BOTH_HORIZ);

			var open = (ToolButton) actions.get_action ("OpenAction").create_tool_item ();
			open.is_important = true;
			toolbar.insert (open, -1);

			var save = (ToolButton) actions.get_action ("SaveAction").create_tool_item ();
			save.is_important = true;
			toolbar.insert (save, -1);

			toolbar.insert (new SeparatorToolItem (), -1);

			var prefs = (ToolButton) actions.get_action ("PreferencesAction").create_tool_item ();
			prefs.is_important = true;
			toolbar.insert (prefs, -1);

			toolbar.insert (new SeparatorToolItem (), -1);

			var quit = (ToolButton) actions.get_action ("QuitAction").create_tool_item ();
			quit.is_important = true;
			toolbar.insert (quit, -1);

			// This separator forces the rest of the items to the end of the toolbar.
			var sep = new SeparatorToolItem ();
			sep.set_expand (true);
			sep.draw = false;
			toolbar.insert (sep, -1);

			var about = (ToolButton) actions.get_action ("AboutAction").create_tool_item ();
			about.is_important = true;
			toolbar.insert (about, -1);

			message_bar = new MessageBar (_("A new version of Spek is available on <a href=\"http://www.spek-project.org\">www.spek-project.org</a>"));

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
			vbox.pack_start (toolbar, false, true, 0);
			vbox.pack_start (message_bar, false, true, 0);
			vbox.pack_start (spectrogram, true, true, 0);
			add (vbox);
			toolbar.show_all ();
			spectrogram.show_all ();
			vbox.show ();
			show ();

			ui.insert_action_group (actions, 0);
			try {
				ui.add_ui_from_string (get_accel_ui_string (actions), -1);
			} catch {
			}
			add_accel_group (ui.get_accel_group ());

			// Set up Drag and Drop
			drag_dest_set (this, DestDefaults.ALL, DEST_TARGET_ENTRIES, DragAction.COPY);
			drag_data_received.connect (on_dropped);

			if (file_name != null) {
				open_file (file_name);
			}

			try {
#if VALA_0_12
				Thread.create<void*> (check_version, false);
#else
				Thread.create (check_version, false);
#endif
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

		private void on_open_action () {
			var chooser = new FileChooserDialog (
				_("Open File"), this, FileChooserAction.OPEN,
				STOCK_CANCEL, ResponseType.CANCEL,
				STOCK_OPEN, ResponseType.ACCEPT, null);
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

		private void on_save_action () {
			var chooser = new FileChooserDialog (
				_("Save Spectrogram"), this, FileChooserAction.SAVE,
				STOCK_CANCEL, ResponseType.CANCEL,
				STOCK_SAVE, ResponseType.ACCEPT, null);
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

		private void on_prefs_action () {
			var dlg = new PreferencesDialog ();
			dlg.transient_for = this;
			dlg.run ();
		}

		private void on_quit_action () {
			destroy ();
		}

		private void on_about_action () {
			string[] authors = {
				"Primary Development:",
				"\tAlexander Kojevnikov (maintainer)",
				"",
				"Contributors:",
				"\tFabian Deutsch",
				"\tStefan Kost"
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
//			dlg.set ("documenters", documenters);
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
				Idle.add (() => { message_bar.show_all (); return false; });
			}

			// Update the preferences.
			prefs.check_update = check;
			prefs.last_update = (int) today.get_julian ();
			prefs.save ();
			return null;
		}

		private string get_accel_ui_string (Gtk.ActionGroup actions) {
			var sb = new StringBuilder ();
			sb.append ("<ui>");
			foreach (var action in actions.list_actions ()) {
				sb.append ("<accelerator name=\"%s\" action=\"%s\" />".printf (action.name, action.name));
			}
			sb.append ("</ui>");
			return sb.str;
		}
	}
}
