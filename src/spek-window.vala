/* spek-window.vala
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

using Gdk;
using Gtk;

namespace Spek {
	public class Window : Gtk.Window {

		private Spectrogram spectrogram;
		private string cur_dir;
		private FileFilter filter_all;
		private FileFilter filter_audio;
		private FileFilter filter_png;

		public Window () {
			title = _("Spek - Acoustic Spectrum Analyser");
			set_default_icon_name ("spek");
			set_default_size (640, 480);
			destroy.connect (Gtk.main_quit);

			var group = new AccelGroup ();
			add_accel_group (group);

			var toolbar = new Toolbar ();
			toolbar.set_style (ToolbarStyle.BOTH_HORIZ);

			var open = new ToolButton.from_stock (STOCK_OPEN);
			open.is_important = true;
			open.add_accelerator (
				"clicked", group, 'O', ModifierType.CONTROL_MASK, AccelFlags.VISIBLE);
			open.clicked.connect (on_open_clicked);
			toolbar.insert (open, -1);

			var save = new ToolButton.from_stock (STOCK_SAVE);
			save.is_important = true;
			save.add_accelerator (
				"clicked", group, 'S', ModifierType.CONTROL_MASK, AccelFlags.VISIBLE);
			save.clicked.connect (on_save_clicked);
			toolbar.insert (save, -1);

			toolbar.insert (new SeparatorToolItem (), -1);

			var quit = new ToolButton.from_stock (STOCK_QUIT);
			quit.is_important = true;
			quit.add_accelerator (
				"clicked", group, 'Q', ModifierType.CONTROL_MASK, AccelFlags.VISIBLE);
			quit.clicked.connect (s => destroy());
			toolbar.insert (quit, -1);

			// This separator forces the rest of the items to the end of the toolbar.
			var sep = new SeparatorToolItem ();
			sep.set_expand (true);
			sep.draw = false;
			toolbar.insert (sep, -1);

			var about = new ToolButton.from_stock (STOCK_ABOUT);
			about.is_important = true;
			about.add_accelerator ("clicked", group, keyval_from_name ("F1"), 0, AccelFlags.VISIBLE);
			about.clicked.connect (on_about_clicked);
			toolbar.insert (about, -1);

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
			foreach (var mime_type in audio_mime_types) {
				filter_audio.add_mime_type (mime_type);
			}

			var vbox = new VBox (false, 0);
			vbox.pack_start (toolbar, false, true, 0);
			vbox.pack_start (spectrogram, true, true, 0);
			add (vbox);
			show_all ();
		}

		private void on_open_clicked () {
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
				var file_name = chooser.get_filename ();
				cur_dir = Path.get_dirname (file_name);
				spectrogram.open (file_name);
			}
			chooser.destroy ();
		}

		private void on_save_clicked () {
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

		private void on_about_clicked () {
			string[] authors = {
				"Primary Development:",
				"\tAlexander Kojevnikov (maintainer) <alexander@kojevnikov.com>",
				"",
				"Contributors:",
				"\tFabian Deutsch"
			};
			string[] artists = {
				"Olga Vasylevska"
			};
			string license = "Copyright (C) 2010  Alexander Kojevnikov";
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

			show_about_dialog (
				this,
				"program-name", "Spek",
				"version", Config.PACKAGE_VERSION,
				"copyright", _("Copyright \xc2\xa9 2010 Alexander Kojevnikov"),
				"comments", title,
				"authors", authors,
//				"documenters", documenters,
				"artists", artists,
				"website-label", _("Spek Website"),
				"website", "http://spek-project.org/",
				"license", license,
				"wrap-license", true,
				"logo-icon-name", "spek",
				"translator-credits", _("translator-credits"));
		}

		private string[] audio_mime_types = {
			"audio/3gpp",
			"audio/ac3",
			"audio/AMR",
			"audio/AMR-WB",
			"audio/basic",
			"audio/midi",
			"audio/mp4",
			"audio/mpeg",
			"audio/ogg",
			"audio/prs.sid",
			"audio/vnd.rn-realaudio",
			"audio/x-aiff",
			"audio/x-ape",
			"audio/x-flac",
			"audio/x-gsm",
			"audio/x-it",
			"audio/x-m4a",
			"audio/x-matroska",
			"audio/x-mod",
			"audio/x-mp3",
			"audio/x-mpeg",
			"audio/x-ms-asf",
			"audio/x-ms-asx",
			"audio/x-ms-wax",
			"audio/x-ms-wma",
			"audio/x-musepack",
			"audio/x-pn-aiff",
			"audio/x-pn-au",
			"audio/x-pn-wav",
			"audio/x-pn-windows-acm",
			"audio/x-realaudio",
			"audio/x-real-audio",
			"audio/x-sbc",
			"audio/x-speex",
			"audio/x-tta",
			"audio/x-wav",
			"audio/x-wavpack",
			"audio/x-vorbis",
			"audio/x-vorbis+ogg",
			"audio/x-xm",
			"application/x-flac"
		};
	}
}
