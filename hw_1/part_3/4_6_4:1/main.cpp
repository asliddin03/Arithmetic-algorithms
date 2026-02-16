#include <iostream>
#include <vector>
#include <cmath>

using std::cin;
using std::cout;
using std::vector;

double horner(const vector<double>& a, double x) {
    double res = 0;
    for (int i = a.size() - 1; i >= 0; --i) {
        res = res * x + a[i];
    }
    return res;
}

int main() {
    cout << "ПРОГРАММА: 4.6.4 упр.1 — вычисление нечётного многочлена u(x)=u1*x+u3*x^3+...\n";

    int n;
    cout << "Введите максимальную степень n: ";
    cin >> n;

    vector<double> u(n + 1);
    cout << "Введите коэффициенты u0..u" << n << " (u_k при x^k):\n";
    for (int k = 0; k <= n; k++) {
        cout << "u[" << k << "] = ";
        cin >> u[k];
    }

    double x;
    cout << "Введите x: ";
    cin >> x;

    vector<double> v; // v0=u1, v1=u3, ...
    for (int k = 1; k <= n; k += 2) v.push_back(u[k]);

    double t = x * x;
    double vt = v.empty() ? 0 : horner(v, t);
    double ans = x * vt;

    cout << "\nРЕЗУЛЬТАТ:\n";
    cout << "t = x^2 = " << t << "\n";
    cout << "u(x) = x*v(t) = " << ans << "\n";

}