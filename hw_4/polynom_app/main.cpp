#include "Polynomial.h"
#include "PolynomialIO.h"

#include <exception>
#include <iostream>
#include <string>
#include <vector>

using std::cin;
using std::cout;

int main() {
    try {
        int k = 5;
        vector<string> vars = {"x", "y"};

        PolynomialTrie f(vars, k);
        PolynomialTrie g(vars, k);

        cout << "Текущее кольцо коэффициентов: Z_" << k << "\n";
        printMenu();
        cout << "Подсказка: 100 — показать меню снова.\n";

        while (true) {
            cout << "\nВведите команду: ";

            int cmd;
            cin >> cmd;

            if (cmd == 0) {
                cout << "Программа завершена.\n";
                break;
            }

            if (cmd == 100) {
                cout << "\nТекущее кольцо коэффициентов: Z_" << k << "\n";
                printMenu();
                continue;
            }

            switch (cmd) {
                case 1: {
                    k = readModulus();
                    vars = readVariableNames();

                    f = PolynomialTrie(vars, k);
                    g = PolynomialTrie(vars, k);

                    cout << "Настройки обновлены: работаем в Z_" << k << "\n";
                    break;
                }

                case 2: {
                    f = readPolynomial(vars, k, "f");
                    cout << "f = " << f.toString() << "\n";
                    break;
                }

                case 3: {
                    g = readPolynomial(vars, k, "g");
                    cout << "g = " << g.toString() << "\n";
                    break;
                }

                case 4: {
                    cout << "f = " << f.toString() << "\n";
                    cout << "g = " << g.toString() << "\n";
                    break;
                }

                case 5: {
                    PolynomialTrie result = f;
                    result += g;
                    cout << "f + g = " << result.toString() << "\n";
                    break;
                }

                case 6: {
                    PolynomialTrie result = f;
                    result -= g;
                    cout << "f - g = " << result.toString() << "\n";
                    break;
                }

                case 7: {
                    PolynomialTrie result = f;
                    result *= g;
                    cout << "f * g = " << result.toString() << "\n";
                    break;
                }

                case 8: {
                    cout << "f = " << f.toString() << "\n";
                    printSupport(f);
                    break;
                }

                case 9: {
                    if (f == g) {
                        cout << "f и g равны.\n";
                    } else {
                        cout << "f и g не равны.\n";
                    }
                    break;
                }

                case 10: {
                    vector<int> point(vars.size());

                    cout << "Введите точку A:\n";
                    for (size_t i = 0; i < vars.size(); ++i) {
                        cout << vars[i] << " = ";
                        cin >> point[i];
                    }

                    const int value = f.evaluateAt(point);
                    cout << "f(A) = " << value << " (mod " << k << ")\n";
                    break;
                }

                case 11: {
                    const int d = f.homeDegree();

                    if (f.isZero()) {
                        cout << "f = 0. В программе считаем нулевой многочлен однородным степени 0.\n";
                    } else if (d == -1) {
                        cout << "f не является однородным многочленом.\n";
                    } else {
                        cout << "f является однородным многочленом степени " << d << ".\n";
                    }
                    break;
                }

                case 12: {
                    int p;
                    cout << "Введите степень p: ";
                    cin >> p;

                    const auto [h, rest] = f.splitByDegree(p);

                    cout << "h = " << h.toString() << "\n";
                    cout << "g = " << rest.toString() << "\n";
                    cout << "Проверка: h + g = " << (h + rest).toString() << "\n";
                    break;
                }

                default: {
                    cout << "Неизвестная команда. Введите 100, чтобы снова показать меню.\n";
                    break;
                }
            }
        }
    } catch (const std::exception& ex) {
        std::cerr << "Ошибка: " << ex.what() << "\n";
    }

    return 0;
}