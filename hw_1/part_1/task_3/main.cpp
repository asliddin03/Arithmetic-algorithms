#include <iostream>
#include <iomanip>
#include <vector>

using std::cin;
using std::cout;
using std::vector;

int main() {
    cout << "Введите степень многочлена k: ";
    int k;
    cin >> k;

    long double a, B;
    cout << "Введите a и B: ";
    cin >> a >> B;

    vector<long double> coef_f(k + 1);
    cout << "Введите коэффициенты для f(x):\n";
    for (int i = 0; i <= k; ++i) {
        cout << "f[" << i << "] = ";
        cin >> coef_f[i];
    }

    // Таблица биномиальных коэффициентов (таблица Паскаля)
    vector <vector<long double>> binom_coef(k + 1, vector<long double>(k + 1, 0));
    for (int n = 0; n <= k; ++n) {
        binom_coef[n][0] = binom_coef[n][n] = 1.0;
        for (int j = 1; j < n; ++j) {
            binom_coef[n][j] = binom_coef[n - 1][j - 1] + binom_coef[n - 1][j];
        }
    }

    vector<long double> coef_ck(k + 1);
    long double delta = B - a;
    for (int j = 0; j <= k; ++j) {
        long double sum = 0;
        long double powDelta = 1.0;
        for (int i = j; i <= k; ++i) {
            sum += coef_f[i] * binom_coef[i][j] * powDelta;
            powDelta *= delta;
        }
        coef_ck[j] = sum;
    }

    cout << "Найденные коэффициенты c(k)\n";
    cout << std::fixed << std::setprecision(12);
    for (int i = 0; i <= k; ++i) {
        cout << coef_ck[i] << (i == k ? '\n' : ' ');
    }
}