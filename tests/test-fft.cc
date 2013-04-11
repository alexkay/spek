/* test-fft.cc
 *
 * Copyright (C) 2013  Alexander Kojevnikov <alexander@kojevnikov.com>
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

#include "spek-fft.h"

#include "test.h"

static void test_const()
{
    FFT fft;
    for (int nbits = 4; nbits < 16; ++nbits) {
        auto plan = fft.create(nbits);
        test("input size", 1 << nbits, plan->get_input_size());
        test("output size", (1 << nbits) / 2 + 1, plan->get_output_size());

        // Test zero input.
        for (int i = 0; i < plan->get_input_size(); ++i) {
            plan->set_input(i, 0.0f);
        }
        plan->execute();
        bool ok = true;
        for (int i = 0; i < plan->get_output_size(); ++i) {
            if (plan->get_output(i) > -1e12f) {
                ok = false;
                break;
            }
        }
        test("zero input", true, ok);

        // Test DC input.
        for (int i = 0; i < plan->get_input_size(); ++i) {
            plan->set_input(i, 1.0f);
        }
        plan->execute();
        ok = true;
        test("dc component", 0.0f, plan->get_output(0));
        for (int i = 1; i < plan->get_output_size(); ++i) {
            if (plan->get_output(i) > -1e12f) {
                ok = false;
                break;
            }
        }
        test("dc input", true, ok);
    }
}

void test_fft()
{
    run("fft const", test_const);
}
