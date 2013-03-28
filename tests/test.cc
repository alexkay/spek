/* test.cc
 *
 * Copyright (C) 2012-2013  Alexander Kojevnikov <alexander@kojevnikov.com>
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

#include "test.h"

int g_passes = 0;
int g_total = 0;

int main()
{
    std::cerr << "-------------" << std::endl;

    test_audio();
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
