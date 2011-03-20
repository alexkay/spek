/* spek-preferences-dialog.vala
 *
 * Copyright (C) 2011  Alexander Kojevnikov <alexander@kojevnikov.com>
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

using Gtk;

namespace Spek {
	public class PreferencesDialog : Gtk.Dialog {
		public PreferencesDialog () {
			title = _("Preferences");
			modal = true;
			resizable = false;
			window_position = WindowPosition.CENTER_ON_PARENT;

			var alignment = new Alignment (0.5f, 0.5f, 1f, 1f);
			alignment.set_padding (12, 12, 12, 12);
			var box = new VBox (false, 0);

			var general_box = new VBox (false, 6);
			// TRANSLATORS: The name of a section in the Preferences dialog.
			var general_label = new Label (_("General"));
			var attributes = new Pango.AttrList ();
			attributes.insert (Pango.attr_weight_new (Pango.Weight.BOLD));
			general_label.attributes = attributes;
			general_label.xalign = 0;
			general_box.pack_start (general_label, false, false, 0);
			var general_alignment = new Alignment (0.5f, 0.5f, 1f, 1f);
			general_alignment.left_padding = 12;
			var general_subbox = new VBox (false, 6);
			var language_box = new HBox (false, 12);
			var language_label = new Label.with_mnemonic (_("_Language:"));
			language_box.pack_start (language_label, false, false, 0);
			var language_combo = new ComboBox.text ();
			language_combo.append_text (_("(system default)"));
			language_combo.append_text ("English");
			language_label.mnemonic_widget = language_combo;
			language_box.pack_start (language_combo, false, false, 0);
			general_subbox.pack_start(language_box, false, false, 0);
			var check_update = new CheckButton.with_mnemonic (_("Check for _updates"));
			general_subbox.pack_start (check_update, false, false, 0);
			general_alignment.add (general_subbox);
			general_box.pack_start (general_alignment, false, false, 0);

			box.pack_start (general_box, false, false, 0);
			alignment.add (box);
			vbox.pack_start (alignment, false, false, 0);
			vbox.show_all ();

			add_button (STOCK_CLOSE, ResponseType.CLOSE);
			set_default_response (ResponseType.CLOSE);
			response.connect (on_response);
		}

		private void on_response (Dialog dialog, int response_id) {
			destroy ();
		}
	}
}