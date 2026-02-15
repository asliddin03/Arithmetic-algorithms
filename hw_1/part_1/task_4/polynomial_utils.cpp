#include "polynomial_utils.h"
#include <cmath>

using std::vector;

void trim(vector<long double>& p) {
    while (p.size() > 1 && fabsl(p.back()) < EPS) p.pop_back();
}

long double evalPolynomial(vector<long double> p, long double x) {
    long double res = 0;
    for (int i = p.size() - 1; i >= 0; i--) {
        res = res * x + p[i];
    }
    return res;
}

vector<long double> deriv(vector<long double>& p) {
    trim(p);
    int n = p.size() - 1;
    if (n <= 0) {
        return {0};
    }
    vector<long double> d(n);
    for (int i = 1; i <= n; i++) {
        d[i - 1] = p[i] * i;
    }
    trim(d);
    return d;
}

int orderZero(vector<long double>& p, long double A) {
    trim(p);
    int k = 0;
    while (p.size() > 1 && fabsl(evalPolynomial(p, A)) < EPS) {
        p = deriv(p);
        k++;
    }

    return k;
}

vector<long double> derivK(vector<long double> p, int k) {
    for (int i = 0; i < k; ++i) {
        p = deriv(p);
    }
    return p;
}

int degree(vector<long double>& p) {
    trim(p);
    return p.size() - 1;
}