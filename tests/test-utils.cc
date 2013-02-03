/* test-utils.cc
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

#include <spek-utils.h>

#include "test.h"

int main()
{
    ensure(0 == spek_vercmp("1.2.3", "1.2.3"), "1.2.3 == 1.2.3");
    ensure(1 == spek_vercmp("1.2.3", "1.2.2"), "1.2.3 > 1.2.2");
    ensure(-1 == spek_vercmp("1.2.2", "1.2.3"), "1.2.2 < 1.2.3");
    ensure(1 == spek_vercmp("1.2.3", "1"), "1.2.3 > 1");
    ensure(1 == spek_vercmp("1.2.3", "1."), "1.2.3 > 1.");
    ensure(1 == spek_vercmp("1.2.3", "1.2"), "1.2.3 > 1.2");
    ensure(1 == spek_vercmp("1.2.3", "1.2."), "1.2.3 > 1.2.");
    ensure(1 == spek_vercmp("1.15.3", "1.2"), "1.15.3 > 1.2");
    ensure(1 == spek_vercmp("2", "1.2.2"), "2 > 1.2.2");
    ensure(1 == spek_vercmp("1.2.3", ""), "1.2.3 > ''");
    ensure(0 == spek_vercmp("", ""), "'' == ''");
    ensure(0 == spek_vercmp("123", "123"), "123 == 123");
    ensure(-1 == spek_vercmp("0.2.3", "1"), "0.2.3 < 1");
    ensure(-1 == spek_vercmp("0.9.8", "0.10.1"), "0.9.8 < 0.10.1");
    ensure(-1 == spek_vercmp("1.200", "2.20"), "1.200 < 2.20");
    ensure(-1 == spek_vercmp("1.0.0", "2.0.0"), "1.0.0 < 2.0.0");
    ensure(-1 == spek_vercmp("1.0.0", "1.0.1"), "1.0.0 < 1.0.1");

    return 0;
}
