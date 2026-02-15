#include "limitInf.h"
#include <sstream>
#include <iomanip>
#include <iostream>

using std::vector;
using std::pair;

pair<string, string> limitInf(vector<long double> f, vector<long double> g) {
    trim(f);
    trim(g);
    if (g.size() == 1 && fabsl(g[0]) < EPS) {
        return {"не определено: g(x) = 0", "не определено: g(x) = 0"};
    }

    int nf = degree(f);
    int ng = degree(g);

    if (nf < ng) return {"0", "0"};

    long double af = f.back();
    long double ag = g.back();

    if (nf == ng) {
        long double val = af / ag;
        std::ostringstream out; out << std::fixed << std::setprecision(12) << val;
        return {out.str(), out.str()};
    }

    int k = nf - ng;
    long double coef = af / ag;

    string plus, minus;
    if (k % 2 == 0) {
        plus = minus = (coef > 0 ? "+бесконечность" : "-бесконечность");
    } else {
        plus  = (coef > 0 ? "+бесконечность" : "-бесконечность");
        minus = (coef > 0 ? "-бесконечность" : "+бесконечность");
    }
    return {plus, minus};
}