#include <iostream>
#include <vector>
#include <complex>
#include <cmath>

using std::cin;
using std::cout;
using std::vector;
using std::complex;

struct Ops { long long mul = 0, add = 0; };

complex<double> hornerComplex(const vector<complex<double> >& u, complex<double> z, Ops& ops) {
    int n = u.size() - 1;
    complex<double> acc = u[n];
    for (int k = n - 1; k >= 0; --k) {
        ops.mul += 4;
        ops.add += 2;
        ops.add += 2;
        acc = acc * z + u[k];
    }
    return acc;
}

complex<double> scheme3(const vector<complex<double> >& u, complex<double> z, Ops& ops) {
    int n = u.size() - 1;
    if (n == 0) return u[0];

    double x = z.real(), y = z.imag();
    double r = 2.0 * x;
    double s = x * x + y * y;

    complex<double> a = u[n];
    complex<double> b = u[n - 1];

    for (int j = 2; j <= n; j++) {
        ops.mul += 2;
        ops.add += 2;
        complex<double> aNew = b + r * a;

        ops.mul += 2;
        ops.add += 2;
        complex<double> bNew = u[n - j] - s * a;

        a = aNew;
        b = bNew;
    }

    ops.mul += 4;
    ops.add += 2;
    ops.add += 2;
    return z * a + b;
}

int main() {
    cout << "ПРОГРАММА: 4.6.4 упр.4 — сравнение Хорнера и схемы (3) для u(z).\n";
    cout << "Ввод: u0..un (Re Im) и z=x+iy.\n\n";

    int n;
    cout << "Введите степень n: ";
    cin >> n;

    vector<complex<double> > u(n + 1);
    cout << "Введите коэффициенты u0..u" << n << " как пары Re Im:\n";
    for (int k = 0; k <= n; k++) {
        double re, im;
        cout << "u[" << k << "] = ";
        cin >> re >> im;
        u[k] = {re, im};
    }

    double x, y;
    cout << "Введите z как x y (z=x+iy): ";
    cin >> x >> y;
    complex<double> z(x, y);

    Ops oH, oS;
    complex<double> vH = hornerComplex(u, z, oH);
    complex<double> vS = scheme3(u, z, oS);

    cout << "\nРЕЗУЛЬТАТ:\n";
    cout << "Horner u(z)    = " << vH << "\n";
    cout << "Scheme(3) u(z) = " << vS << "\n";
    cout << "Разница |H-S|  = " << abs(vH - vS) << "\n\n";

    cout << "Оценка вещественных операций:\n";
    cout << "Horner:   mul=" << oH.mul << " add=" << oH.add << "\n";
    cout << "Scheme(3):mul=" << oS.mul << " add=" << oS.add << "\n";

    return 0;
}
