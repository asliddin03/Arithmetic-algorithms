#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::vector;

int main() {
    cout << "ПРОГРАММА: 4.6.4 упр.3 — вычисление U(x,y)=sum u_{i,j} x^i y^j при i+j<=n.\n";
    cout << "Метод: вложенный Горнер (сначала по y, затем по x).\n\n";

    int n;
    cout << "Введите n (максимальная общая степень): ";
    cin >> n;

    double x, y;
    cout << "Введите x: ";
    cin >> x;
    cout << "Введите y: ";
    cin >> y;

    // Треугольное хранение: coef[i][j] для j<=n-i
    vector<vector<double>> coef(n + 1);
    for (int i = 0; i <= n; i++) {
        coef[i].assign(n - i + 1, 0.0);
    }

    int m;
    cout << "Введите количество ненулевых коэффициентов m: ";
    cin >> m;

    cout << "Введите m строк: i j u_ij (где i+j<=n):\n";
    for (int t = 0; t < m; t++) {
        int i, j;
        double val;
        cout << "Терм " << (t + 1) << ": ";
        cin >> i >> j >> val;
        if (i < 0 || j < 0 || i > n || i + j > n) {
            cout << "Ошибка: нужно 0<=i<=n и 0<=j<=n-i.\n";
            return 0;
        }
        coef[i][j] = val;
    }

    // 1) A_i(y)
    vector<double> A(n + 1, 0.0);
    for (int i = 0; i <= n; i++) {
        int degY = n - i;
        double acc = coef[i][degY];
        for (int j = degY - 1; j >= 0; j--) {
            acc = acc * y + coef[i][j];
        }
        A[i] = acc;
    }

    // 2) Horner по x
    double res = A[n];
    for (int i = n - 1; i >= 0; i--) {
        res = res * x + A[i];
    }

    cout << "\nРЕЗУЛЬТАТ:\n";
    cout << "U(x,y) = " << res << "\n";

    return 0;
}
