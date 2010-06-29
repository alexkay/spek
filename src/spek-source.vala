/* spek-source.vala
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

using Gst;

namespace Spek {
	public class Source : GLib.Object {

		public string file_name { get; construct; }
		public int bands { get; construct; }
		public int samples { get; construct; }
		public int threshold { get; construct; }
		// TODO: file a bug, cannot s/set/construct/
		public DataCallback data_cb { get; set; }
		public InfoCallback info_cb { get; set; }

		public int64 duration { get; private set; default = 0; }
		public int rate { get; private set; default = 0; }
		public string audio_codec { get; private set; default = null; }
		public uint bitrate { get; private set; default = 0; }
		public int channels { get; private set; default = 0; }
		public int depth { get; private set; default = 0; }

		public delegate void DataCallback (int sample, float[] values);
		public delegate void InfoCallback ();

		private Gst.Pipeline pipeline = null;
		private Element spectrum = null;
		private Pad pad = null;
		private int sample;
		private float[] values;
		private uint watch_id;

		public Source (
			string file_name, int bands, int samples, int threshold,
			DataCallback data_cb, InfoCallback info_cb) {
			GLib.Object (file_name: file_name, bands: bands, samples: samples, threshold: threshold);
			this.data_cb = data_cb;
			this.info_cb = info_cb;
		}

		public void stop () {
			// Need to manually remove the bus watch, otherwise `this` is never unreferenced.
			if (watch_id > 0) {
				GLib.Source.remove (watch_id);
				watch_id = 0;
			}
			if (pipeline != null) {
				pipeline.set_state (State.NULL);
				pipeline = null;
			}
		}

		construct {
			sample = 0;
			values = new float[bands];

			// TODO: Check for gst errors, in particular test the situation when
			// `spectrum` (good), `decodebin` (base) or `fakesink` (core) plugins are not available.
			pipeline = new Gst.Pipeline ("pipeline");
			var filesrc = ElementFactory.make ("filesrc", null);
			var decodebin = ElementFactory.make ("decodebin", null);
			pipeline.add_many (filesrc, decodebin);
			filesrc.link (decodebin);
			filesrc.set ("location", file_name);

			// decodebin's src pads are not constructed immediately.
			// See gst-plugins-base/tree/gst/playback/test6.c
			Signal.connect (
				decodebin, "new-decoded-pad",
				(GLib.Callback) on_new_decoded_pad, this);

			this.watch_id = pipeline.get_bus ().add_watch (on_bus_watch);
			if (pipeline.set_state (State.PAUSED) == StateChangeReturn.ASYNC) {
				pipeline.get_state (null, null, -1);
			}

			// Get the sample rate.
			var caps = pad.get_caps ();
			for (int i = 0; i < caps.get_size (); i++) {
				var structure = caps.get_structure (i);
				Gst.Value? val;
				int n;
				if (rate == 0 && (val = structure.get_value ("rate")) != null) {
					if (val.type () == typeof (int)) {
						rate = val.get_int ();
					} else if (val.type () == typeof (IntRange)) {
						rate = val.get_int_range_max ();
					}
				}
				if (channels == 0 && (val = structure.get_value ("channels")) != null) {
					if (val.type () == typeof (int)) {
						channels = val.get_int ();
					} else if (val.type () == typeof (IntRange)) {
						channels = val.get_int_range_max ();
					}
				}
				if (depth == 0 && structure.get_int ("depth", out n)) {
					depth = n;
				}
			}

			// Get the duration.
			// TODO: replace with Pad.query_duration when bgo#617260 is fixed
			var query = new Query.duration (Format.TIME);
			pad.query (query);
			Format format;
			int64 duration;
			query.parse_duration (out format, out duration);
			this.duration = duration;
			spectrum.set ("interval", duration / (samples + 1));

			pipeline.set_state (State.PLAYING);
		}

		// TODO: get rid of the last parameter when bgo#615979 is fixed
		private static void on_new_decoded_pad (
			Element decodebin, Pad new_pad, bool last, Source source) {
			if (spectrum != null) {
				// We want to construct the spectrum element only for the first decoded pad.
				return;
			}

			// The audioconvert takes care of caps that `spectrum` can't handle,
			// for example streams with 24-bit depth.
			var convert = ElementFactory.make ("audioconvert", null);
			source.pipeline.add (convert);
			var sinkpad = convert.get_static_pad ("sink");
			source.pad = new_pad;
			source.pad.link (sinkpad);
			convert.set_state (State.PAUSED);

			source.spectrum = ElementFactory.make ("spectrum", null);
			source.pipeline.add (source.spectrum);
			convert.link (source.spectrum);
			source.spectrum.set ("bands", source.bands);
			source.spectrum.set ("threshold", source.threshold);
			source.spectrum.set ("message-magnitude", true);
			source.spectrum.set ("post-messages", true);
			source.spectrum.set_state (State.PAUSED);

			var fakesink = ElementFactory.make ("fakesink", null);
			source.pipeline.add (fakesink);
			source.spectrum.link (fakesink);
			fakesink.set_state (State.PAUSED);
		}

		private bool on_bus_watch (Bus bus, Message message) {
			var structure = message.get_structure ();
			switch (message.type ) {
			case MessageType.ELEMENT:
				if (structure.get_name () == "spectrum") {
					var magnitudes = structure.get_value ("magnitude");
					for (int i = 0; i < bands; i++) {
						values[i] = magnitudes.list_get_value (i).get_float ();
					}
					data_cb (sample++, values);
				}
				break;
			case MessageType.TAG:
				TagList tag_list;
				message.parse_tag (out tag_list);
				tag_list.foreach (on_tag);
				break;
			}
			return true;
		}

		private void on_tag (TagList tag_list, string tag) {
			switch (tag) {
			case TAG_AUDIO_CODEC:
				string s = null;
				if (audio_codec == null && tag_list.get_string (tag, out s)) {
					audio_codec = s;
					info_cb ();
				}
				break;
			case TAG_BITRATE:
				uint u = 0;
				if (bitrate == 0 && tag_list.get_uint (tag, out u)) {
					bitrate = u;
					info_cb ();
				}
				break;
			}
		}
	}
}