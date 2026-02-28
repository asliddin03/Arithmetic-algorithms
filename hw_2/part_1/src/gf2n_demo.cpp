#include "gf2n_demo.h"
#include "gf2n_functions.h"
#include <iostream>

using std::cout;
using std::hex;
using std::dec;

static const int  n   = 8;
static const u128 mod = (u128)0x11B;
static const u64  a   = 0x57;
static const u64  b   = 0x83;

static void print_common_header() {
    cout << "GF(2^n) с модулем f(x)\n";
    cout << "  n = " << n << "\n";
    cout << "  f(x) = " << u128_to_hex(mod) << " = x^8 + x^4 + x^3 + x + 1\n";
    cout << "  a = 0x" << hex << a << dec << "  =>  a(x) = " << polynom_to_string_u64(a) << "\n";
    cout << "  b = 0x" << hex << b << dec << "  =>  b(x) = " << polynom_to_string_u64(b) << "\n";
    cout << "------------------------------------------------------------\n";
}

void print_task_1() {
    cout << "ПУНКТ 1\n";
    cout << "Требуется: представить элемент GF(2^n) в полиномиальной форме и обратно.\n";
    print_common_header();
    cout << "Вывод (полиномиальная форма):\n";
    cout << "  a(x) = " << polynom_to_string_u64(a) << "\n";
    cout << "  b(x) = " << polynom_to_string_u64(b) << "\n\n";
}

void print_task_2() {
    cout << "ПУНКТ 2\n";
    cout << "Требуется: умножить два двоичных многочлена степени не выше 32.\n";
    cout << "Операция: умножение в GF(2)[x] БЕЗ взятия по модулю f(x).\n";
    print_common_header();
    u128 raw = polynom_mul_bin_u32(a, b);
    cout << "Результат:\n";
    cout << "  a(x) * b(x) (без модуля) = " << u128_to_hex(raw) << "\n\n";
}

void print_task_3() {
    cout << "ПУНКТ 3\n";
    cout << "Требуется: умножение двух элементов поля GF(2^n).\n";
    cout << "Операция: c = (a(x) * b(x)) mod f(x).\n";
    print_common_header();
    u64 c = gf_mul(a, b, mod, n);
    cout << "Результат:\n";
    cout << "  c = 0x" << hex << c << dec << "\n";
    cout << "  c(x) = " << polynom_to_string_u64(c) << "\n\n";
}

void print_task_4() {
    cout << "ПУНКТ 4\n";
    cout << "Требуется: расширенный алгоритм Евклида для GF(2)[x].\n";
    cout << "Операция: найти gcd(a,f) и коэффициенты X,Y: a*X + f*Y = gcd(a,f).\n";
    print_common_header();
    EGCD eg = polynom_ext_gcd((u128)a, mod);
    cout << "Результат:\n";
    cout << "  gcd(a,f) = " << u128_to_hex(eg.g) << " (ожидаем 0x1 при a!=0)\n";
    cout << "  X(x) (коэффициент при a) = " << u128_to_hex(eg.x) << "\n";
    cout << "  Y(x) (коэффициент при f) = " << u128_to_hex(eg.y) << "\n\n";
}

void print_task_5() {
    cout << "ПУНКТ 5\n";
    cout << "Требуется: найти мультипликативный обратный inv(a) в GF(2^n).\n";
    cout << "Операция: inv(a) такое, что a * inv(a) = 1 (mod f).\n";
    print_common_header();
    u64 inva = gf_inverse(a, mod, n);
    cout << "Результат:\n";
    cout << "  inv(a) = 0x" << hex << inva << dec << "\n";
    cout << "  inv(a)(x) = " << polynom_to_string_u64(inva) << "\n";
    u64 check = gf_mul(a, inva, mod, n);
    cout << "Проверка:\n";
    cout << "  a * inv(a) mod f = 0x" << hex << check << dec << " (должно быть 0x1)\n\n";
}

void print_all_tasks() {
    print_task_1();
    print_task_2();
    print_task_3();
    print_task_4();
    print_task_5();
}