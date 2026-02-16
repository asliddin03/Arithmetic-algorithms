#include <iostream>
#include <vector>
#include <cmath>

using std::cin;
using std::cout;
using std::vector;

double horner(const vector<double>& u, double x) {
    double acc = 0;
    for (int i = u.size() - 1; i >= 0; --i) acc = acc * x + u[i];
    return acc;
}

int main() {
    cout << "ПРОГРАММА: 4.6.4 упр.8 — вычисление u(x) через падающие факториалы (x)_k.\n";
    cout << "Шаги: строим S(m,k), переводим в коэффициенты a_k, считаем sum a_k*(x)_k.\n\n";

    int n;
    cout << "Введите степень n: ";
    cin >> n;

    vector<double> u(n + 1);
    cout << "Введите коэффициенты u0..u" << n << " (u_m при x^m):\n";
    for (int m = 0; m <= n; m++) {
        cout << "u[" << m << "] = ";
        cin >> u[m];
    }

    double x;
    cout << "Введите x: ";
    cin >> x;

    // 1) Stirling S(m,k)
    vector<vector<double> > S(n + 1, vector<double>(n + 1, 0.0));
    S[0][0] = 1.0;
    for (int m = 1; m <= n; m++) {
        for (int k = 1; k <= m; k++) {
            S[m][k] = k * S[m - 1][k] + S[m - 1][k - 1];
        }
    }

    // 2) a_k = sum_{m=k..n} u_m * S(m,k)
    vector<double> a(n + 1, 0.0);
    for (int k = 0; k <= n; k++) {
        double sum = 0.0;
        for (int m = k; m <= n; m++) sum += u[m] * S[m][k];
        a[k] = sum;
    }

    // 3) sum a_k*(x)_k
    double fk = 1.0;
    double ans = a[0] * fk;

    for (int k = 1; k <= n; k++) {
        fk *= (x - (k - 1));
        ans += a[k] * fk;
    }

    cout << "\nПЕРЕВОД В БАЗИС (x)_k:\n";
    for (int k = 0; k <= n; k++) {
        cout << "a[" << k << "] = " << a[k] << "\n";
    }

    cout << "\nРЕЗУЛЬТАТ:\n";
    cout << "u(x) через (x)_k = " << ans << "\n";

    return 0;
}
