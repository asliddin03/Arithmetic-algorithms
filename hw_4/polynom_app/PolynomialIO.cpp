#include "PolynomialIO.h"

#include <iostream>
#include <stdexcept>

using namespace std;

int readModulus() {
    int k;
    cout << "Введите модуль k для Z_k: ";
    cin >> k;

    if (k <= 0) {
        throw runtime_error("Модуль k должен быть положительным.");
    }

    return k;
}

vector<string> readVariableNames() {
    int n;
    cout << "Введите число переменных n: ";
    cin >> n;

    if (n <= 0) {
        throw runtime_error("Число переменных должно быть положительным.");
    }

    vector<string> vars(n);
    cout << "Введите имена переменных:\n";

    for (int i = 0; i < n; ++i) {
        cout << "name[" << i << "] = ";
        cin >> vars[i];
    }

    return vars;
}

PolynomialTrie readPolynomial(const vector<string>& vars, int mod, const string& polyName) {
    PolynomialTrie p(vars, mod);

    int m;
    cout << "Введите число мономов многочлена " << polyName << ": ";
    cin >> m;

    if (m < 0) {
        throw runtime_error("Число мономов не может быть отрицательным.");
    }

    for (int i = 0; i < m; ++i) {
        int coef;
        vector<int> exponents(vars.size());

        cout << "\nМоном #" << (i + 1) << "\n";
        cout << "Коэффициент: ";
        cin >> coef;

        for (size_t j = 0; j < vars.size(); ++j) {
            cout << "Степень переменной " << vars[j] << ": ";
            cin >> exponents[j];
        }

        p.addMonomial(coef, exponents);
    }

    return p;
}

void printMenu() {
    cout << "\n================= МЕНЮ =================\n";
    cout << "1. Ввести/изменить модуль k и переменные\n";
    cout << "2. Ввести многочлен f\n";
    cout << "3. Ввести многочлен g\n";
    cout << "4. Показать f и g\n";
    cout << "5. Пункт a: f + g\n";
    cout << "6. Пункт a: f - g\n";
    cout << "7. Пункт a: f * g\n";
    cout << "8. Пункт b: supp(f)\n";
    cout << "9. Пункт c: проверить f == g\n";
    cout << "10. Пункт d: вычислить f(A)\n";
    cout << "11. Пункт e: проверить однородность f\n";
    cout << "12. Пункт f: выделить однородную часть степени p\n";
    cout << "0. Выход\n";
    cout << "100. Показать меню снова\n";
    cout << "========================================\n";
}

void printSupport(const PolynomialTrie& f) {
    const auto supp = f.support();

    if (supp.empty()) {
        cout << "supp(f) = пустое множество\n";
        return;
    }

    cout << "supp(f):\n";
    for (const auto& exps : supp) {
        cout << "(";
        for (size_t i = 0; i < exps.size(); ++i) {
            cout << exps[i];
            if (i + 1 != exps.size()) {
                cout << ", ";
            }
        }
        cout << ")\n";
    }
}