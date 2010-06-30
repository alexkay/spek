/* spek-pipeline.vala
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

namespace Spek {
	public class Pipeline {
		private Audio.Context cx;
		public string description { get; private set; }

		public Pipeline (string file_name) {
			cx = new Audio.Context (file_name);

			// Build the description string.
			string[] items = {};
			if (cx.codec_name != null) {
				items += cx.codec_name;
			}
			if (cx.bit_rate != 0) {
				items += _("%d kbps").printf (cx.bit_rate / 1000);
			}
			if (cx.sample_rate != 0) {
				items += _("%d Hz").printf (cx.sample_rate);
			}
			// Show bits per sample only if there is no bitrate.
			if (cx.bits_per_sample != 0 && cx.bit_rate == 0) {
				items += ngettext (
					"%d bit", "%d bits", cx.bits_per_sample).printf (cx.bits_per_sample);
			}
			if (cx.channels != 0) {
				items += ngettext ("%d channel", "%d channels", cx.channels).
					printf (cx.channels);
			}
			description = items.length > 0 ? string.joinv (", ", items) : "";

			if (cx.error != null) {
				// TRANSLATORS: first %s is the error message, second %s is stream description.
				description = _("%s: %s").printf (cx.error, description);
			}
		}

		public string file_name {
			get { return cx.file_name; }
		}

		public int bit_rate {
			get { return cx.bit_rate; }
		}

		public int sample_rate {
			get { return cx.sample_rate; }
		}

		public int channels {
			get { return cx.channels; }
		}
	}
}
