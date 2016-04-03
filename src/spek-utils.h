#pragma once

inline int spek_max(int a, int b)
{
    return a > b ? a : b;
}

inline int spek_min(int a, int b)
{
    return a < b ? a : b;
}

// Compare version numbers, e.g. 1.9.2 < 1.10.0
int spek_vercmp(const char *a, const char *b);
