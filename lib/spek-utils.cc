/* spek-utils.cc
 *
 * Copyright (C) 2012  Alexander Kojevnikov <alexander@kojevnikov.com>
 *
 * Spek is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Spek is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Spek.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <limits.h>
#include <stdlib.h>

#include "spek-utils.h"

int spek_vercmp(const char *a, const char *b)
{
    assert(a && b);

    if (!*a && !*b) {
        return 0;
    }
    if (!*a) {
        return -1;
    }
    if (!*b) {
        return 1;
    }

    char *i, *j;
    while(1) {
        i = j = NULL;
        long x = strtol(a, &i, 10);
        long y = strtol(b, &j, 10);

        if (x < y) {
            return -1;
        }
        if (x > y) {
            return 1;
        }

        if (!*i && !*j) {
            return 0;
        }
        if (!*i) {
            return -1;
        }
        if (!*j) {
            return 1;
        }

        a = i + 1;
        b = j + 1;
    }
}
