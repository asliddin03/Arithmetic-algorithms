#include <iostream>
#include <stdexcept>
#include <utility>

std::pair<double, double> complex_div_6ops(double a0, double a1, double b0, double b1) {
    if (b0 == 0.0 && b1 == 0.0) {
        throw std::invalid_argument("Деление на ноль: z2 = 0 + 0i");
    }

    if (std::fabs(b0) >= std::fabs(b1)) {
        const double r = b1 / b0;
        const double den = b0 + b1 * r;

        const double x = (a0 + a1 * r) / den;
        const double y = (a1 - a0 * r) / den;
        return {x, y};
    } else {
        const double r = b0 / b1;
        const double den = b1 + b0 * r;

        const double x = (a1 + a0 * r) / den;
        const double y = (-a0 + a1 * r) / den;
        return {x, y};
    }
}

int main() {
    double a0, a1, b0, b1;

    std::cout << "Введите действительную часть z1: a0 = ";
    std::cin >> a0;
    std::cout << "Введите мнимую часть z1: a1 = ";
    std::cin >> a1;

    std::cout << "Введите действительную часть z2: b0 = ";
    std::cin >> b0;
    std::cout << "Введите мнимую часть z2: b1 = ";
    std::cin >> b1;

    try {
        auto [x, y] = complex_div_6ops(a0, a1, b0, b1);
        std::cout << "\nРезультат:\n";
        std::cout << "z1 / z2 = " << x << " + " << y << "i\n";
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
        return 1;
    }

    return 0;
}