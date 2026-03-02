#include <iostream>
#include <iomanip>

struct Complex {
    double re;
    double im;
};

Complex mul_naive(double a, double b, double c, double d) {
    // (a+bi)(c+di) = (ac-bd) + (ad+bc)i
    return {a*c - b*d, a*d + b*c};
}

Complex mul_3mul_5add(double a, double b, double c, double d) {
    double s1 = a + b;   // 1-е сложение
    double s2 = a - b;   // 2-е сложение

    double cd = c + d;   // 3-е сложение

    double p1 = s1 * c;  // 1-е умножение
    double p2 = s2 * d;  // 2-е умножение
    double p3 = b  * cd; // 3-е умножение

    double re = p1 - p3; // 4-е сложение
    double im = p2 + p3; // 5-е сложение

    return {re, im};
}

int main() {
    std::cout << "Введите a b c d для произведения (a+bi)(c+di): ";
    double a, b, c, d;
    if (!(std::cin >> a >> b >> c >> d)) {
        std::cerr << "Ошибка ввода.\n";
        return 1;
    }

    Complex x = mul_naive(a, b, c, d);
    Complex y = mul_3mul_5add(a, b, c, d);

    std::cout << std::fixed << std::setprecision(12);

    std::cout << "\nОбычный способ:          (" << x.re << ") + (" << x.im << ")i\n";
    std::cout << "Способ 3 умн. + 5 слож.: (" << y.re << ") + (" << y.im << ")i\n";

    double err_re = std::abs(x.re - y.re);
    double err_im = std::abs(x.im - y.im);

    std::cout << "\nРазница по действит. части: " << err_re << "\n";
    std::cout << "Разница по мнимой части:    " << err_im << "\n";

    const double eps = 1e-9;
    if (err_re < eps && err_im < eps) {
        std::cout << "OK: результаты совпадают (с точностью eps=" << eps << ").\n";
    } else {
        std::cout << "ВНИМАНИЕ: результаты отличаются (возможны эффекты округления).\n";
    }

    return 0;
}