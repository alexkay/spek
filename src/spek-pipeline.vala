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
		public string file_name { get; private set; }
		public string audio_codec { get; private set; }

		public Pipeline (string file_name) {
			this.file_name = file_name;
		}

		public void open () {
			Audio.open (file_name);
		}
	}
}
