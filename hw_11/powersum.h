#pragma once


#include <vector>

struct Result {
    long double value;
    long long operations;
};

long double fastPower(long double base, int power, long long& operations);

Result sumDirect(int n, int p);
Result sumAsymptotic(int n, int p);
Result sumExactFormula(int n, int p);

long double binomial(int n, int k, long long& operations);
std::vector<long double> bernoulliNumbers(int p, long long& operations);