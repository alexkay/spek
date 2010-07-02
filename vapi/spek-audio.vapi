[CCode (cprefix = "SpekAudio", lower_case_cprefix = "spek_audio_", cheader_filename = "spek-audio.h")]
namespace Spek.Audio {
	[Compact]
	[CCode (free_function = "spek_audio_close")]
	public class Context {
		public string file_name;
		public string codec_name;
		public string error;
		public int bit_rate;
		public int sample_rate;
		public int bits_per_sample;
		public int width;
		public bool fp;
		public int channels;
		public int buffer_size;

		[CCode (cname = "spek_audio_open")]
		public Context (string file_name);
		[CCode (cname = "spek_audio_read")]
		public int read ([CCode (array_length = false)] uint8[] buffer);
	}
	public static void init ();
}
