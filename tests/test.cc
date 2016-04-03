#include "test.h"

int g_passes = 0;
int g_total = 0;

int main()
{
    std::cerr << "-------------" << std::endl;

    test_audio();
    test_fft();
    test_utils();

    if (g_passes < g_total) {
        std::cerr << "\x1b[31;1m" << (g_total - g_passes) << "/" << g_total;
        std::cerr << " tests failed" << "\x1b[0m" << std::endl;
        return -1;
    } else {
        std::cerr << "\x1b[32;1m" << g_total << " tests passed" << "\x1b[0m" << std::endl;
        return 0;
    }
}

void run(const std::string& message, std::function<void ()> func)
{
    std::cerr << message << std::endl;
    func();
}
