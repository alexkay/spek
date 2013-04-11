/* test.h
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

#ifndef TEST_H_
#define TEST_H_

#include <cmath>
#include <functional>
#include <iostream>
#include <string>

void run(const std::string& message, std::function<void ()> func);

extern int g_total;
extern int g_passes;

template<class T> bool equal(const T& a, const T& b)
{
    return a == b;
}

template<> inline bool equal<double>(const double& a, const double& b)
{
    return std::abs(a - b) < 1e-8;
}

template<class T> void test(const std::string& message, const T& expected, const T& actual)
{
    g_total++;
    if (equal(expected, actual)) {
        g_passes++;
    } else {
        std::cerr << "\x1b[31;1mFAIL:\x1b[0m " << message;
        std::cerr << ", expected: " << "\x1b[32;1m" << expected << "\x1b[0m";
        std::cerr << ", actual: " << "\x1b[31;1m" << actual << "\x1b[0m" << std::endl;
    }
}

void test_audio();
void test_fft();
void test_utils();

#endif
