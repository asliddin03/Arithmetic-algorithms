#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::vector;

// p(x) умножить на (x + x0)
vector<double> mulByLinear(const vector<double>& p, double x0) {
    int deg = p.size() - 1;
    vector<double> q(deg + 2, 0.0);

    // p*x: q[i+1] += p[i]
    for (int i = 0; i <= deg; i++) {
        q[i + 1] += p[i];
    }

    // p*x0: q[i] += p[i]*x0
    for (int i = 0; i <= deg; i++) {
        q[i] += p[i] * x0;
    }

    return q;
}

int main() {
    cout << "ПРОГРАММА: 4.6.4 упр.2 — нахождение коэффициентов v(x)=u(x+x0).\n";
    cout << "Метод: Горнер в кольце многочленов (p = p*(x+x0) + u_k).\n\n";

    int n;
    cout << "Введите степень n: ";
    cin >> n;

    vector<double> u(n + 1);
    cout << "Введите коэффициенты u0..u" << n << " (u_k при x^k):\n";
    for (int k = 0; k <= n; k++) {
        cout << "u[" << k << "] = ";
        cin >> u[k];
    }

    double x0;
    cout << "Введите x0: ";
    cin >> x0;

    // p(x) = u_n
    vector<double> p(1, u[n]);

    for (int k = n - 1; k >= 0; k--) {
        p = mulByLinear(p, x0);
        p[0] += u[k];
    }

    cout << "\nРЕЗУЛЬТАТ:\n";
    cout << "Коэффициенты v0..v" << n << " для v(x)=u(x+x0):\n";
    for (int i = 0; i <= n; i++) {
        cout << "v[" << i << "] = " << p[i] << "\n";
    }

    return 0;
}
