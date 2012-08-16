/* spek-spectrogram.vala
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

using Cairo;
using Gdk;
using Gtk;
using Pango;

namespace Spek {
    class Spectrogram : DrawingArea {

        public void save (string file_name) {
            Allocation allocation;
            get_allocation (out allocation);
            var surface = new ImageSurface (Format.RGB24, allocation.width, allocation.height);
            draw (new Cairo.Context (surface));
            surface.write_to_png (file_name);
        }
    }
}
