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

        private ImageSurface image;

        public Spectrogram () {

        public void save (string file_name) {
            Allocation allocation;
            get_allocation (out allocation);
            var surface = new ImageSurface (Format.RGB24, allocation.width, allocation.height);
            draw (new Cairo.Context (surface));
            surface.write_to_png (file_name);
        }

        private void draw (Cairo.Context cr) {
            int text_width, text_height;

            if (image != null) {
                // Prepare to draw the rulers.
                cr.set_source_rgb (1, 1, 1);
                cr.set_line_width (1);
                cr.set_antialias (Antialias.NONE);
                layout.set_font_description (FontDescription.from_string (
                    "Sans " + (8 * FONT_SCALE).to_string ()));
                layout.set_width (-1);

                // Time ruler.
                var duration_seconds = (int) pipeline.duration;
                var time_ruler = new Ruler (
                    Ruler.Position.BOTTOM,
                    // TODO: i18n
                    "00:00",
                    {1, 2, 5, 10, 20, 30, 1*60, 2*60, 5*60, 10*60, 20*60, 30*60},
                    duration_seconds,
                    1.5,
                    unit => (w - LPAD - RPAD) * unit / duration_seconds,
                    p => p,
                    // TODO: i18n
                    unit => "%d:%02d".printf (unit / 60, unit % 60));
                cr.translate (LPAD, h - BPAD);
                time_ruler.draw (cr, layout);
                cr.identity_matrix ();

                // Frequency ruler.
                var freq = pipeline.sample_rate / 2;
                var rate_ruler = new Ruler (
                    Ruler.Position.LEFT,
                    // TRANSLATORS: keep "00" unchanged, it's used to calc the text width
                    _("00 kHz"),
                    {1000, 2000, 5000, 10000, 20000},
                    freq,
                    3.0,
                    unit => (h - TPAD - BPAD) * unit / freq,
                    p => p,
                    unit => _("%d kHz").printf (unit / 1000));
                cr.translate (LPAD, TPAD);
                rate_ruler.draw (cr, layout);
                cr.identity_matrix ();
            }

            // Prepare to draw the ruler.
            cr.set_source_rgb (1, 1, 1);
            cr.set_line_width (1);
            cr.set_antialias (Antialias.NONE);
            layout.set_font_description (FontDescription.from_string (
                "Sans " + (8 * FONT_SCALE).to_string ()));
            layout.set_width (-1);

            // Spectral density.
            var density_ruler = new Ruler (
                Ruler.Position.RIGHT,
                // TRANSLATORS: keep "-00" unchanged, it's used to calc the text width
                _("-00 dB"),
                {1, 2, 5, 10, 20, 50},
                -THRESHOLD,
                3.0,
                unit => -(h - TPAD - BPAD) * unit / THRESHOLD,
                p => h - TPAD - BPAD - p,
                unit => _("%d dB").printf (-unit));
            cr.translate (w - RPAD + GAP + RULER, TPAD);
            density_ruler.draw (cr, layout);
            cr.identity_matrix ();
        }
    }
}
