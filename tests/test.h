#pragma once

#include <cmath>
#include <cstdio>
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
