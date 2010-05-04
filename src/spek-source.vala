using Gst;

namespace Spek {
	public class Source : GLib.Object {

		public string file_name { get; construct; }

		private Pipeline pipeline;

		public Source (string file_name) {
			GLib.Object (file_name: file_name);
		}

		~Source () {
			pipeline.set_state (State.NULL);
		}

		construct {
			pipeline = new Pipeline ("pipeline");
			var filesrc = ElementFactory.make ("filesrc", null);
			var decodebin = ElementFactory.make ("decodebin", null);
			pipeline.add_many (filesrc, decodebin);
			filesrc.link (decodebin);
			filesrc.set ("location", file_name);

			Signal.connect (decodebin, "new-decoded-pad", (GLib.Callback) on_new_decoded_pad, pipeline);

			pipeline.get_bus ().add_watch (on_bus_watch);

			if (StateChangeReturn.ASYNC == pipeline.set_state (State.PLAYING)) {
				pipeline.get_state (null, null, -1);
			}

			/*
			unowned Iterator it = decodebin.iterate_src_pads ();
			void *data;
			while (it.next (out data) == IteratorResult.OK) {
				var pad = (Pad) data;
				var caps = pad.get_caps ();
				var structure = caps.get_structure (0);
				stdout.printf ("structure=%s\n", structure.to_string ());
			}
			*/
		}

		private static void on_new_decoded_pad (
			Element decodebin, Pad new_pad, bool last, Pipeline pipeline) {
			var spectrum = ElementFactory.make ("spectrum", null);
			pipeline.add (spectrum);
			var sinkpad = spectrum.get_static_pad ("sink");
			new_pad.link (sinkpad);
			spectrum.set ("bands", 10);
			spectrum.set ("interval", 1000000000); // 1 sec
			spectrum.set ("threshold", -99);
			spectrum.set ("message-magnitude", true);
			spectrum.set ("post-messages", true);
			spectrum.set_state (State.PAUSED);

			var fakesink = ElementFactory.make ("fakesink", null);
			pipeline.add (fakesink);
			spectrum.link (fakesink);
			fakesink.set_state (State.PAUSED);
		}

		private static bool on_bus_watch (Bus bus, Message message) {
			var structure = message.get_structure ();
			if (message.type == MessageType.ELEMENT &&
				structure.get_name () == "spectrum") {

				ClockTime endtime = 0;
				// TODO: binding must be fixed: `out endtime`
				structure.get_clock_time ("endtime", (ClockTime) (void *) (&endtime));
				stdout.printf ("%d:", endtime / 1000000000);

				var magnitudes = structure.get_value ("magnitude");
				for (int i=0; i<10; i++) {
					var mag = magnitudes.list_get_value (i);
					stdout.printf (" %.2f", mag.get_float ());
				}
				stdout.printf ("\n");
			}
			return true;
		}
	}
}