/* test.h
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

#ifndef TEST_H_
#define TEST_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

inline void ensure(bool test, const char *message)
{
    if (!test) {
        printf("ERROR: %s\n", message);
        exit(1);
    }
}

#endif
