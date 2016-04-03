#pragma once

#include <stdint.h>

enum palette {
    PALETTE_SPECTRUM,
    PALETTE_MONO,
    PALETTE_COUNT,
    PALETTE_DEFAULT = PALETTE_SPECTRUM,
};

uint32_t spek_palette(enum palette palette, double level);
