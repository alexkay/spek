#include "spek-utils.h"

#include "test.h"

static void test_vercmp()
{
    test<int>("1.2.3 == 1.2.3", 0, spek_vercmp("1.2.3", "1.2.3"));
    test("1.2.3 > 1.2.2", 1, spek_vercmp("1.2.3", "1.2.2"));
    test("1.2.2 < 1.2.3", -1, spek_vercmp("1.2.2", "1.2.3"));
    test("1.2.3 > 1", 1, spek_vercmp("1.2.3", "1"));
    test("1.2.3 > 1.", 1, spek_vercmp("1.2.3", "1."));
    test("1.2.3 > 1.2", 1, spek_vercmp("1.2.3", "1.2"));
    test("1.2.3 > 1.2.", 1, spek_vercmp("1.2.3", "1.2."));
    test("1.15.3 > 1.2", 1, spek_vercmp("1.15.3", "1.2"));
    test("2 > 1.2.2", 1, spek_vercmp("2", "1.2.2"));
    test("1.2.3 > ''", 1, spek_vercmp("1.2.3", ""));
    test("'' == ''", 0, spek_vercmp("", ""));
    test("123 == 123", 0, spek_vercmp("123", "123"));
    test("0.2.3 < 1", -1, spek_vercmp("0.2.3", "1"));
    test("0.9.8 < 0.10.1", -1, spek_vercmp("0.9.8", "0.10.1"));
    test("1.200 < 2.20", -1, spek_vercmp("1.200", "2.20"));
    test("1.0.0 < 2.0.0", -1, spek_vercmp("1.0.0", "2.0.0"));
    test("1.0.0 < 1.0.1", -1, spek_vercmp("1.0.0", "1.0.1"));
}

void test_utils()
{
    run("vercmp", test_vercmp);
}
