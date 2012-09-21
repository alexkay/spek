/* spek-utils.h
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

#ifndef SPEK_UTILS_H_
#define SPEK_UTILS_H_

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

#endif
