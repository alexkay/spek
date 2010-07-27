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

		private Fft.Plan fft;
		private int nfft; // Size of the FFT transform.
		private const int NFFT = 64; // Number of FFTs to pre-fetch.
		private int input_size;
		private int input_pos;
		private float[] input;
		private float[] output;

		private unowned Thread reader_thread = null;
		private unowned Thread worker_thread;
		private Mutex reader_mutex;
		private Cond reader_cond;
		private Mutex worker_mutex;
		private Cond worker_cond;
		private bool worker_done = false;
		private bool quit = false;

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
				items += _("%d kbps").printf ((cx.bit_rate + 500) / 1000);
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
			} else {
				this.sample_rate = cx.sample_rate;
				this.nfft = 2 * bands - 2;
				this.fft = new Fft.Plan (nfft, threshold);
				this.input_size = nfft * (NFFT * 2 + 1);
				this.input = new float[input_size];
				this.output = new float[bands];
				this.cx.start (samples);
			}
		}

		~Pipeline () {
			stop ();
		}

		public void start () {
			stop ();

			if (cx.error != null) return;

			input_pos = 0;
			reader_mutex = new Mutex ();
			reader_cond = new Cond ();
			worker_mutex = new Mutex ();
			worker_cond = new Cond ();

			try {
				reader_thread = Thread.create (reader_func, true);
			} catch (ThreadError e) {
				stop ();
			}
		}

		public void stop () {
			if (reader_thread != null) {
				lock (quit) {
					quit = true;
				}
				reader_thread.join ();
				quit = false;
				reader_thread = null;
			}
		}

		private void * reader_func () {
			int pos = 0, prev_pos = 0;
			int block_size = cx.width * cx.channels / 8;
			int size;

			try {
				worker_thread = Thread.create (worker_func, true);
			} catch (ThreadError e) {
				return null;
			}

			while ((size = cx.read ()) > 0) {
				lock (quit) if (quit) break;

				uint8 *buffer = (uint8 *) cx.buffer;
				while (size >= block_size) {
					input[pos] = average_input (buffer);
					buffer += block_size;
					size -= block_size;
					pos = (pos + 1) % input_size;

					// Wake up the worker if we have enough data.
					if ((pos > prev_pos ? pos : pos + input_size) - prev_pos == nfft * NFFT) {
						reader_sync (prev_pos = pos);
					}
				}
				assert (size == 0);
			}

			if (pos != prev_pos) {
				// Process the remaining data.
				reader_sync (pos);
			}
			// Force the worker to quit.
			reader_sync (-1);
			worker_thread.join ();

			return null;
		}

		private void reader_sync (int pos) {
			reader_mutex.lock ();
			while (!worker_done) reader_cond.wait (reader_mutex);
			worker_done = false;
			reader_mutex.unlock ();

			worker_mutex.lock ();
			input_pos = pos;
			worker_cond.signal ();
			worker_mutex.unlock ();
		}

		private void * worker_func () {
			int sample = 0;
			int64 frames = 0;
			int64 num_fft = 0;
			int64 acc_error = 0;
			float cf = 2f * (float) Math.PI / nfft;
			int head = 0, tail = 0;
			int prev_head = 0;

			Memory.set (output, 0, sizeof (float) * bands);

			while (true) {
				reader_mutex.lock ();
				worker_done = true;
				reader_cond.signal ();
				reader_mutex.unlock ();

				worker_mutex.lock ();
				while (tail == input_pos) worker_cond.wait (worker_mutex);
				tail = input_pos;
				worker_mutex.unlock ();

				if (tail == -1) {
					return null;
				}

				while (true) {
					head = (head + 1) % input_size;
					if (head == tail) {
						head = prev_head;
						break;
					}
					frames++;

					// If we have enough frames for an FFT or we
					// have all frames required for the interval run
					// an FFT. In the last case we probably take the
					// FFT of frames that we already handled.
					bool int_full = acc_error < cx.error_base && frames == cx.frames_per_interval;
					bool int_over = acc_error >= cx.error_base && frames == 1 + cx.frames_per_interval;
					if (frames % nfft == 0 || int_full || int_over) {
						prev_head = head;
						for (int i = 0; i < nfft; i++) {
							float val = input[(input_size + head - nfft + i) % input_size];
							// Hamming window.
							val *= 0.53836f - 0.46164f * Math.cosf (cf * i);
							fft.input[i] = val;
						}
						fft.execute ();
						num_fft++;
						for (int i = 0; i < bands; i++) {
							output[i] += fft.output[i];
						}
					}
					// Do we have the FFTs for one interval?
					if (int_full || int_over) {
						if (int_over) {
							acc_error -= cx.error_base;
						} else {
							acc_error += cx.error_per_interval;
						}

						for (int i = 0; i < bands; i++) {
							output[i] /= num_fft;
						}

						if (sample == samples) break;
						cb (sample++, output);

						Memory.set (output, 0, sizeof (float) * bands);
						frames = 0;
						num_fft = 0;
					}
				}
			}
		}

		private float average_input (uint8 *buffer) {
			int channels = cx.channels;
			float res = 0f;
			if (cx.fp) {
				if (cx.width == 32) {
					float *p = (float *) buffer;
					for (int i = 0; i < channels; i++) {
						res += p[i];
					}
				} else {
					assert (cx.width == 64);
					double *p = (double *) buffer;
					for (int i = 0; i < channels; i++) {
						res += (float) p[i];
					}
				}
			} else {
				if (cx.width == 16) {
					int16 *p = (int16 *) buffer;
					for (int i = 0; i < channels; i++) {
						res += p[i] / (float) int16.MAX;
					}
				} else {
					assert (cx.width == 32);
					int32 *p = (int32 *) buffer;
					for (int i = 0; i < channels; i++) {
						res += p[i] / (float) int32.MAX;
					}
				}
			}
			return res / channels;
		}
	}
}
