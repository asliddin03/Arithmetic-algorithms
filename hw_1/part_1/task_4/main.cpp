#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include "limitAtA.h"
#include "limitInf.h"

using std::cin;
using std::cout;
using std::vector;
using std::string;

const long double EPS = 1e-12L;

int main() {
    int n;
    cout << "Введите степень многочлена f n: ";
    cin >> n;
    vector<long double> f(n + 1);
    cout << "Введите коэффициенты для f(x):\n";
    for (int i = 0; i <= n; i++) {
        cin >> f[i];
    }

    int m;
    cout << "Введите степень многочлена g m: ";
    cin >> m;
    vector<long double> g(m + 1);
    cout << "Введите коэффициенты для g(x):\n";
    for (int i = 0; i <= m; i++) {
        cin >> g[i];
    }

    long double A;
    cin >> A;

    cout << "lim_{x->A} R(x) = " << limitAtA(f, g, A) << "\n";

    auto [p, mn] = limitInf(f, g);
    cout << "lim_{x->+inf} R(x) = " << p  << "\n";
    cout << "lim_{x->-inf} R(x) = " << mn << "\n";
}