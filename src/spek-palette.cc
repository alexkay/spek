#include "spek-palette.h"

// Modified version of Dan Bruton's algorithm:
// http://www.physics.sfasu.edu/astro/color/spectra.html
uint32_t spek_palette_spectrum(double level)
{
    level *= 0.6625;
    double r = 0.0, g = 0.0, b = 0.0;
    if (level >= 0 && level < 0.15) {
        r = (0.15 - level) / (0.15 + 0.075);
        g = 0.0;
        b = 1.0;
    } else if (level >= 0.15 && level < 0.275) {
        r = 0.0;
        g = (level - 0.15) / (0.275 - 0.15);
        b = 1.0;
    } else if (level >= 0.275 && level < 0.325) {
        r = 0.0;
        g = 1.0;
        b = (0.325 - level) / (0.325 - 0.275);
    } else if (level >= 0.325 && level < 0.5) {
        r = (level - 0.325) / (0.5 - 0.325);
        g = 1.0;
        b = 0.0;
    } else if (level >= 0.5 && level < 0.6625) {
        r = 1.0;
        g = (0.6625 - level) / (0.6625 - 0.5f);
        b = 0.0;
    }

    // Intensity correction.
    double cf = 1.0;
    if (level >= 0.0 && level < 0.1) {
        cf = level / 0.1;
    }
    cf *= 255.0;

    // Pack RGB values into a 32-bit uint.
    uint32_t rr = (uint32_t) (r * cf + 0.5);
    uint32_t gg = (uint32_t) (g * cf + 0.5);
    uint32_t bb = (uint32_t) (b * cf + 0.5);
    return (rr << 16) + (gg << 8) + bb;
}
