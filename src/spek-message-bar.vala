/* spek-message-bar.vala
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
 *
 * This class is heavily influenced by its namesake from Banshee.
 * The original code by Aaron Bockover is (c) 2008 Novell, Inc.
 */

using Gtk;

namespace Spek {
	public class MessageBar : HBox {
		private Label label;
		private Gtk.Window win;

		public MessageBar (string message) {
			homogeneous = false;
			spacing = 0;
			border_width = 6;

			win = new Gtk.Window (WindowType.POPUP);
			win.name = "gtk-tooltips";
			win.ensure_style ();
			win.style_set.connect (() => style = win.style);

			label = new Label (null);
			label.use_markup = true;
			label.set_markup (message);
			label.ellipsize = Pango.EllipsizeMode.END;
			label.xalign = 0f;
			label.activate_link.connect (uri => { Platform.show_uri (uri); return true; });
			var button_box = new HBox (false, 0);
			button_box.spacing = 3;
			var close_button = new Button ();
			close_button.image = new Gtk.Image.from_stock (Stock.CLOSE, IconSize.MENU);
			close_button.relief = ReliefStyle.NONE;
			close_button.clicked.connect (() => hide ());

			pack_start (label, true, true, 0);
			pack_start (button_box, false, false, 0);
			pack_start (close_button, false, false, 0);
		}

		private bool changing_style = false;
		protected override void style_set (Style? previous_style) {
			if (changing_style) {
				return;
			}

			changing_style = true;
			style = win.style;
			label.style = style;
			changing_style = false;
		}

		protected override bool expose_event (Gdk.EventExpose event) {
			if (!is_drawable ()) {
				return false;
			}

			var cr = Gdk.cairo_create (event.window);
			var color = style.bg[StateType.NORMAL];
			cr.set_source_rgb (color.red / 65535.0, color.green / 65535.0, color.blue / 65535.0);
			cr.rectangle (allocation.x, allocation.y, allocation.width, allocation.height);
			cr.fill ();
			return base.expose_event (event);
		}
	}
}
