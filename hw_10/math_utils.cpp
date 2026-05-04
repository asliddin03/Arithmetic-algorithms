#include "math_utils.h"

cpp_dec_float_100 epsByDigits(int digits) {
    cpp_dec_float_100 eps = 1;

    for (int i = 0; i < digits; ++i) {
        eps /= 10;
    }

    return eps;
}

// e^r = 1 + r + r^2 / 2! + r^3 / 3! + ...
CalculationResult computeExp(int r, int digits) {
    cpp_dec_float_100 eps = epsByDigits(digits);

    cpp_dec_float_100 sum = 1;
    cpp_dec_float_100 term = 1;

    int terms = 1;
    long long operations = 0;

    for (int n = 1; ; ++n) {
        term *= r;
        operations++;

        term /= n;
        operations++;

        sum += term;
        operations++;

        terms++;

        if (abs(term) < eps) {
            break;
        }
    }

    return {sum, terms, operations};
}

// arctg(x) = x - x^3 / 3 + x^5 / 5 - x^7 / 7 + ...
static CalculationResult arctanSeries(const cpp_dec_float_100 &x, int digits) {
    cpp_dec_float_100 eps = epsByDigits(digits);

    cpp_dec_float_100 x2 = x * x;
    cpp_dec_float_100 term = x;
    cpp_dec_float_100 sum = term;

    int terms = 1;
    long long operations = 1;

    int sign = -1;

    for (int n = 1; ; ++n) {
        term *= x2;

        cpp_dec_float_100 current = term / (2 * n + 1);

        if (sign == 1) {
            sum += current;
        } else {
            sum -= current;
        }

        operations += 3;

        terms++;
        sign *= -1;

        if (abs(current) < eps) {
            break;
        }
    }

    return {sum, terms, operations};
}

// Формула Мачина:
// pi = 16 * arctg(1 / 5) - 4 * arctg(1 / 239)
CalculationResult computePi(int digits) {
    CalculationResult atan1 = arctanSeries(cpp_dec_float_100(1) / 5, digits + 5);

    CalculationResult atan2 = arctanSeries( cpp_dec_float_100(1) / 239, digits + 5);

    cpp_dec_float_100 pi = 16 * atan1.value - 4 * atan2.value;

    int terms = atan1.terms + atan2.terms;

    long long operations = atan1.operations + atan2.operations + 3;

    return {pi, terms, operations};
}