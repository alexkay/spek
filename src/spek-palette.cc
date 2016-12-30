#include <assert.h>
#include <math.h>

#include "spek-palette.h"

// Modified version of Dan Bruton's algorithm:
// http://www.physics.sfasu.edu/astro/color/spectra.html
static uint32_t spectrum(double level)
{
    level *= 0.6625;
    double r = 0.0, g = 0.0, b = 0.0;
    if (level < 0.15) {
        r = (0.15 - level) / (0.15 + 0.075);
        g = 0.0;
        b = 1.0;
    } else if (level < 0.275) {
        r = 0.0;
        g = (level - 0.15) / (0.275 - 0.15);
        b = 1.0;
    } else if (level < 0.325) {
        r = 0.0;
        g = 1.0;
        b = (0.325 - level) / (0.325 - 0.275);
    } else if (level < 0.5) {
        r = (level - 0.325) / (0.5 - 0.325);
        g = 1.0;
        b = 0.0;
    } else if (level < 0.6625) {
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

// The default palette used by SoX and written by Rob Sykes.
static uint32_t sox(double level)
{
    double r = 0.0;
    if (level >= 0.13 && level < 0.73) {
        r = sin((level - 0.13) / 0.60 * M_PI / 2.0);
    } else if (level >= 0.73) {
        r = 1.0;
    }

    double g = 0.0;
    if (level >= 0.6 && level < 0.91) {
        g = sin((level - 0.6) / 0.31 * M_PI / 2.0);
    } else if (level >= 0.91) {
        g = 1.0;
    }

    double b = 0.0;
    if (level < 0.60) {
        b = 0.5 * sin(level / 0.6 * M_PI);
    } else if (level >= 0.78) {
        b = (level - 0.78) / 0.22;
    }

    // Pack RGB values into a 32-bit uint.
    uint32_t rr = (uint32_t) (r * 255.0 + 0.5);
    uint32_t gg = (uint32_t) (g * 255.0 + 0.5);
    uint32_t bb = (uint32_t) (b * 255.0 + 0.5);
    return (rr << 16) + (gg << 8) + bb;
}

static uint32_t mono(double level)
{
    uint32_t v = (uint32_t) (level * 255.0 + 0.5);
    return (v << 16) + (v << 8) + v;
}

uint32_t spek_palette(enum palette palette, double level) {
    switch (palette) {
    case PALETTE_SPECTRUM:
        return spectrum(level);
    case PALETTE_SOX:
        return sox(level);
    case PALETTE_MONO:
        return mono(level);
    default:
        assert(false);
        return 0;
    }
}
