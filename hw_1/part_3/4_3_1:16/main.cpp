#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::vector;

int main() {
    cout << "ПРОГРАММА: 4.3.1 упр.16 — деление большого числа U (в базе b) на v.\n";
    cout << "Вычисляет Q и r: U = Q*v + r.\n\n";

    unsigned long long b, v;
    int n;

    cout << "Введите основание b: ";
    cin >> b;

    cout << "Введите количество цифр n: ";
    cin >> n;

    vector<unsigned long long> u(n);
    cout << "Введите " << n << " цифр U:\n";
    for (int i = 0; i < n; i++) {
        cout << "u[" << i << "] = ";
        cin >> u[i];
        if (u[i] >= b) {
            cout << "Ошибка: u[" << i << "] должна быть < b\n";
            return 0;
        }
    }

    cout << "Введите делитель v (0 < v < b): ";
    cin >> v;
    if (v == 0 || v >= b) {
        cout << "Ошибка: нужно 0 < v < b\n";
        return 0;
    }

    vector<unsigned long long> q(n, 0);
    unsigned long long r = 0;
    for (int i = 0; i < n; i++) {
        r = r * b + u[i];
        q[i] = r / v;
        r = r % v;
    }

    int ind = 0;
    while (ind < n - 1 && q[ind] == 0) ind++;

    cout << "\nРЕЗУЛЬТАТ:\n";
    cout << "Частное Q (цифры в базе b): ";
    for (int i =ind; i < n; i++) {
        cout << q[i];
    }
    cout << "\nОстаток r = " << r << "\n";

    return 0;
}