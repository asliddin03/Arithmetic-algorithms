#include <iostream>
#include <vector>
#include <stdexcept>

using std::cin;
using std::cout;
using std::vector;

static bool is_power_of_two(int n) {
    return n >= 1 && (n & (n - 1)) == 0;
}

static vector<long double> mul_mod_xm_minus_1(const vector<long double>& a,
                                             const vector<long double>& b) {
    int m = (int)a.size();
    vector<long double> c(m, 0.0L);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < m; j++) {
            c[(i + j) % m] += a[i] * b[j];          // x^m = 1
        }
    }
    return c;
}

static vector<long double> mul_mod_xm_plus_1(const vector<long double>& a,
                                            const vector<long double>& b) {
    int m = (int)a.size();
    vector<long double> c(m, 0.0L);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < m; j++) {
            int s = i + j;
            if (s < m) c[s] += a[i] * b[j];
            else       c[s - m] -= a[i] * b[j];     // x^m = -1
        }
    }
    return c;
}

vector<long double> mul_mod_xn_minus_1_via_factorization(const vector<long double>& f,
                                                        const vector<long double>& g) {
    int n = (int)f.size();
    if (n != (int)g.size()) throw std::invalid_argument("Размеры f и g должны совпадать.");
    if (n == 1) return { f[0] * g[0] };

    int m = n / 2;

    vector<long double> f0(m), f1(m), g0(m), g1(m);
    for (int i = 0; i < m; i++) {
        f0[i] = f[i] + f[i + m];   // mod (x^m - 1)
        f1[i] = f[i] - f[i + m];   // mod (x^m + 1)
        g0[i] = g[i] + g[i + m];
        g1[i] = g[i] - g[i + m];
    }

    vector<long double> h0 = mul_mod_xm_minus_1(f0, g0); // правильный mod (x^m - 1)
    vector<long double> h1 = mul_mod_xm_plus_1 (f1, g1); // правильный mod (x^m + 1)

    vector<long double> res(n);
    const long double half = 0.5L;
    for (int i = 0; i < m; i++) {
        long double A = (h0[i] + h1[i]) * half;
        long double B = (h0[i] - h1[i]) * half;
        res[i] = A;
        res[i + m] = B;
    }
    return res;
}

static void print_poly(const vector<long double>& a, const char* name) {
    cout << name << "(x) = ";
    for (int i = 0; i < (int)a.size(); i++) {
        cout << (double)a[i];
        if (i == 1) cout << "x";
        else if (i >= 2) cout << "x^" << i;
        if (i + 1 != (int)a.size()) cout << " + ";
    }
    cout << "\n";
}

int main() {
    int n;
    cout << "Введите n (степень двойки): ";
    if (!(cin >> n)) return 0;
    if (!is_power_of_two(n)) {
        cout << "Ошибка: n должно быть степенью двойки.\n";
        return 1;
    }

    vector<long double> f(n), g(n);

    cout << "Введите коэффициенты многочлена f:\n";
    for (int i = 0; i < n; i++) { cout << "x^" << i << " : "; cin >> f[i]; }

    cout << "Введите коэффициенты многочлена g:\n";
    for (int i = 0; i < n; i++) { cout << "x^" << i << " : "; cin >> g[i]; }

    auto h = mul_mod_xn_minus_1_via_factorization(f, g);

    cout << "\n";
    print_poly(f, "f");
    print_poly(g, "g");
    cout << "h(x) = f(x) * g(x) (mod x^" << n << " - 1)\n";
    print_poly(h, "h");
}