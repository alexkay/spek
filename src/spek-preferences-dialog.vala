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
        // List all languages with a decent (e.g. 80%) number of translated
        // strings. Don't translate language names. Keep the first line intact.
        private static string[,] languages = {
            {"", null},
            {"cs", "Čeština"},
            {"de", "Deutsch"},
            {"en", "English"},
            {"es", "Español"},
            {"fr", "Français"},
            {"it", "Italiano"},
            {"nl", "Nederlands"},
            {"pl", "Polski"},
            {"pt_BR", "Português brasileiro"},
            {"ru", "Русский"},
            {"sv", "Svenska"},
            {"uk", "Українська"}
        };

        public PreferencesDialog () {
            title = _("Preferences");
            modal = true;
            resizable = false;
            window_position = WindowPosition.CENTER_ON_PARENT;
            languages[0,1] = _("(system default)");

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
            int active_language = 0;
            var prefs = Preferences.instance;
            for (int i = 0; i < languages.length[0]; i++) {
                language_combo.append_text (languages[i,1]);
                if (languages[i,0] == prefs.language) {
                    active_language = i;
                }
            }
            language_combo.active = active_language;
            language_combo.changed.connect (
                () => prefs.language = languages[language_combo.active,0]);
            language_label.mnemonic_widget = language_combo;
            language_box.pack_start (language_combo, false, false, 0);
            general_subbox.pack_start(language_box, false, false, 0);
            var check_update = new CheckButton.with_mnemonic (_("Check for _updates"));
            check_update.active = prefs.check_update;
            check_update.toggled.connect (
                () => prefs.check_update = check_update.active);
            general_subbox.pack_start (check_update, false, false, 0);
            general_alignment.add (general_subbox);
            general_box.pack_start (general_alignment, false, false, 0);

            box.pack_start (general_box, false, false, 0);
            alignment.add (box);
            var vbox = (VBox) get_content_area ();
            vbox.pack_start (alignment, false, false, 0);
            vbox.show_all ();

            add_button (Stock.CLOSE, ResponseType.CLOSE);
            set_default_response (ResponseType.CLOSE);
            response.connect (on_response);
        }

        private void on_response (Dialog dialog, int response_id) {
            Preferences.instance.save ();
            destroy ();
        }
    }
}