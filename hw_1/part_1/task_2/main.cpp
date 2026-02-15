#include <iostream>
#include <iomanip>
#include <vector>

using std::cin;
using std::cout;
using std::vector;

const long double EPS = 1e-12L;

struct Polynomial {
 private:
    // coef[i] — коэффициент при x^i
    vector<long double> coef;
 public:
    explicit Polynomial(vector<long double> coef) : coef(coef) {}

    // Степень многочлена
    int degree() const {
        int d = coef.size() - 1;
        while (d > 0 && fabs(coef[d]) < EPS) {
            d--;
        }
        return d;
    }

    // Вычисление значения многочлена в точке х (схема Горнера)
    long double eval(long double x) {
        long double res = 0;
        for (int i = degree(); i >= 0; i--) {
            res = res * x + coef[i];
        }

        return res;
    }

    // Производная многочлена
    Polynomial differential() const {
        int deg = degree();
        if (deg == 0) {
            return Polynomial({0.0});
        }

        vector<long double> coef_d(deg);
        for (int i = 1; i <= deg; i++) {
            coef_d[i - 1] = coef[i] * i;
         }

        return Polynomial(coef_d);
    }

    ~Polynomial() {
        coef.clear();
    }
};

int main() {
    int n;
    cout << "Введите степень многочлена n: ";
    cin >> n;

    vector<long double> coef_f(n + 1);
    cout << "Введите коэффициенты для f(x):\n";
    for (int i = 0; i <= n; i++) {
        cin >> coef_f[i];
    }

    long double a;
    cout << "Введите число a: ";
    cin >> a;

    Polynomial current{coef_f};

    int deg = current.degree();

    vector<long double> coef_ck(deg + 1, 0);
    double factor = 1.0;
    for (int k = 0; k <= deg; k++) {
        coef_ck[k] = current.eval(a) / factor;
        factor *= (k + 1);
        current = current.differential();
    }

    cout << "Найденные коэффициенты c(k)\n";
    cout << std::fixed << std::setprecision(12);
    for (int k = 0; k <= deg; k++) {
        cout << "c[" << k << "] = " << coef_ck[k] << "\n";
    }
}