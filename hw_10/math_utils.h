#pragma once


#include <boost/multiprecision/cpp_dec_float.hpp>

using boost::multiprecision::cpp_dec_float_100;

struct CalculationResult {
    cpp_dec_float_100 value;
    int terms;
    long long operations;
};

cpp_dec_float_100 epsByDigits(int digits);

CalculationResult computeExp(int r, int digits);

CalculationResult computePi(int digits);