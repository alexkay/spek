#include "spek-fft.h"

#include "test.h"

// FFT sizes that avfft can actually handle.
static const int FFT_BITS_MIN = 4;
static const int FFT_BITS_MAX = 15;

static void test_const()
{
    FFT fft;
    for (int nbits = FFT_BITS_MIN; nbits <= FFT_BITS_MAX; ++nbits) {
        auto plan = fft.create(nbits);
        test("input size", 1 << nbits, plan->get_input_size());
        test("output size", (1 << nbits) / 2 + 1, plan->get_output_size());

        // Test zero input.
        for (int i = 0; i < plan->get_input_size(); ++i) {
            plan->set_input(i, 0.0f);
        }
        plan->execute();
        bool silence = true;
        for (int i = 0; i < plan->get_output_size(); ++i) {
            if (plan->get_output(i) > -1e12f) {
                silence = false;
                break;
            }
        }
        test("silence", true, silence);

        // Test DC input.
        for (int i = 0; i < plan->get_input_size(); ++i) {
            plan->set_input(i, 1.0f);
        }
        plan->execute();
        test("dc component", 0.0f, plan->get_output(0));
        silence = true;
        for (int i = 1; i < plan->get_output_size(); ++i) {
            if (plan->get_output(i) > -1e12f) {
                silence = false;
                break;
            }
        }
        test("silence", true, silence);
    }
}

static void test_sine()
{
    FFT fft;
    for (int nbits = FFT_BITS_MIN; nbits <= FFT_BITS_MAX; ++nbits) {
        auto plan = fft.create(nbits);
        int n = plan->get_input_size();
        for (int k = 1; k < n / 2; k *= 2) {
            for (int i = 0; i < n; ++i) {
                plan->set_input(i, sin(k * i * 2.0 * M_PI / n));
            }
            plan->execute();
            test("sine", -602, static_cast<int>(plan->get_output(k) * 100));
            bool silence = true;
            for (int i = 0; i < plan->get_output_size(); ++i) {
                if (i == k) {
                    continue;
                }
                if (plan->get_output(i) > -150.0f) {
                    silence = false;
                    break;
                }
            }
            test("silence", true, silence);
        }
    }
}

void test_fft()
{
    run("fft const", test_const);
    run("fft sine", test_sine);
}
