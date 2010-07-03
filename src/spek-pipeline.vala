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
 *
 * Conversion of decoded samples into an FFT-happy format is heavily
 * influenced by GstSpectrum which is part of gst-plugins-good.
 * The original code:
 * (c) 1999 Erik Walthinsen <omega@cse.ogi.edu>
 * (c) 2006 Stefan Kost <ensonic@users.sf.net>
 * (c) 2007-2009 Sebastian Dröge <sebastian.droege@collabora.co.uk>
 */

namespace Spek {
	public class Pipeline {
		public string description { get; private set; }
		public int sample_rate { get; private set; }
		public double duration { get { return cx.duration; } }
		public delegate void Callback (int sample, float[] values);

		private Audio.Context cx;
		private int bands;
		private int samples;
		private int threshold;
		private Callback cb;

		private uint8[] buffer;
		private Fft.Plan fft;
		private int nfft;
		float[] input;
		float[] output;
		float[] values;

		public Pipeline (string file_name, int bands, int samples, int threshold, Callback cb) {
			this.cx = new Audio.Context (file_name);
			this.bands = bands;
			this.samples = samples;
			this.threshold = threshold;
			this.cb = cb;

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

			this.sample_rate = cx.sample_rate;
			this.buffer = new uint8[cx.buffer_size];
			this.nfft = 2 * bands - 2;
			this.fft = new Fft.Plan (nfft);
			this.input = new float[nfft];
			this.output = new float[bands];
			this.values = new float[bands];
			this.cx.start (samples);
		}

		public void start () {
			int pos = 0;
			int frames = 0;
			int num_fft = 0;
			int sample = 0;
			int size;

			clear_buffers ();

			while ((size = cx.read (this.buffer)) > 0) {
				uint8 *buffer = (uint8 *) this.buffer;
				var block_size = cx.width * cx.channels;
				while (size >= block_size) {
					input[pos] = average_input (buffer);
					buffer += block_size;
					size -= block_size;
					pos = (pos + 1) % nfft;
					frames++;

					// If we have enough frames for an FFT or we
					// have all frames required for the interval run
					// an FFT. In the last case we probably take the
					// FFT of frames that we already handled.
					// TODO: error correction
					if (frames % nfft == 0) {
						for (int i = 0; i < nfft; i++) {
							fft.input[i] = input[(pos + i) % nfft];
						}
						fft.execute ();
						num_fft++;
						for (int i = 0; i < bands; i++) {
							output[i] += fft.output[i];
						}
					}
					// Do we have the FFTs for one interval?
					// TODO: error correction
					if (frames == cx.frames_per_interval) {
						for (int i = 0; i < bands; i++) {
							output[i] /= num_fft;
						}
						cb (sample++, output);
						clear_buffers ();
						frames = 0;
						num_fft = 0;
					}
				}
				assert (size == 0);
			}
		}

		private void clear_buffers () {
			Posix.memset (input, 0, sizeof (float) * nfft);
			Posix.memset (output, 0, sizeof (float) * bands);
		}

		private float average_input (uint8 *buffer) {
			float res = 0f;
			float max_value = cx.bits_per_sample > 1 ? (1UL << (cx.bits_per_sample - 1)) - 1 : 0;
			if (cx.fp && cx.width == 32) {
				float *p = (float *) buffer;
				for (int i = 0; i < cx.channels; i++) {
					res += p[i];
				}
			} else if (cx.fp && cx.width == 64) {
				double *p = (double *) buffer;
				for (int i = 0; i < cx.channels; i++) {
					res += (float) p[i];
				}
			} else if (!cx.fp && cx.width == 32) {
				int32 *p = (int32 *) buffer;
				for (int i = 0; i < cx.channels; i++) {
					res += p[i] / (max_value == 0 ? int32.MAX : max_value);
				}
			} else if (!cx.fp && cx.width == 16) {
				int64 *p = (int64 *) buffer;
				for (int i = 0; i < cx.channels; i++) {
					res += p[i] / (max_value == 0 ? int16.MAX : max_value);
				}
			} else {
				assert_not_reached ();
			}
			return res / cx.channels;
		}
	}
}
