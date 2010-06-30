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

		public Pipeline (string file_name) {
			cx = new Audio.Context (file_name);
			// TODO: check for errors
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
