#pragma once

#include <stdint.h>

enum palette {
    PALETTE_SPECTRUM,
    PALETTE_SOX,
    PALETTE_MONO,
    PALETTE_COUNT,
    PALETTE_DEFAULT = PALETTE_SOX,
};

uint32_t spek_palette(enum palette palette, double level);
