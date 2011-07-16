[CCode (cprefix = "SpekFft", lower_case_cprefix = "spek_fft_", cheader_filename = "spek-fft.h")]
namespace Spek.Fft {
    [Compact]
    [CCode (free_function = "spek_fft_destroy")]
    public class Plan {
        [CCode (array_length = false)]
        public unowned float[] input;
        [CCode (array_length = false)]
        public unowned float[] output;

        [CCode (cname = "spek_fft_plan_new")]
        public Plan (int n, int threshold);
        [CCode (cname = "spek_fft_execute")]
        public void execute ();
    }
}
