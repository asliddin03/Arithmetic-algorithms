#include "limitAtA.h"
#include <sstream>
#include <iomanip>

using std::string;
using std::vector;

string limitAtA(vector<long double> f, vector<long double> g, long double A) {
    trim(f);
    trim(g);

    if (g.size() == 1 && fabsl(g[0]) < EPS) {
        return "не определено: g(x) = 0";
    }

    // ВАЖНО: orderZero портит полином => считаем кратности на копиях
    vector<long double> f0 = f;
    vector<long double> g0 = g;

    int m = orderZero(f0, A);
    int n = orderZero(g0, A);

    // n = 0 => g(A) != 0 => просто подстановка (по исходным f,g)
    if (n == 0) {
        long double val = evalPolynomial(f, A) / evalPolynomial(g, A);
        std::ostringstream out;
        out << std::fixed << std::setprecision(12) << val;
        return out.str();
    }

    // Здесь f и g тоже должны быть исходные
    long double fA = evalPolynomial(f, A);

    // g(A)=0, но f(A)!=0
    if (fabsl(fA) >= EPS) {
        auto gn = derivK(g, n);
        long double gnA = evalPolynomial(gn, A);
        long double s = fA / gnA;

        if (n % 2 == 0) return (s > 0 ? "+бесконечность" : "-бесконечность");
        return "не существует";
    }

    // 0/0
    if (m > n) return "0";

    if (m == n) {
        auto fm = derivK(f, m);
        auto gm = derivK(g, m);
        long double val = evalPolynomial(fm, A) / evalPolynomial(gm, A);
        std::ostringstream out;
        out << std::fixed << std::setprecision(12) << val;
        return out.str();
    }

    // m < n
    int k = n - m;
    auto fm = derivK(f, m);
    auto gn = derivK(g, n);
    long double s = evalPolynomial(fm, A) / evalPolynomial(gn, A);

    if (k % 2 == 0) return (s > 0 ? "+бесконечность" : "-бесконечность");
    return "не существует";
}
