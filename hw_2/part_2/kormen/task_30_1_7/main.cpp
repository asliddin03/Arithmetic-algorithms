#include <iostream>
#include <vector>
#include <complex>
#include <algorithm>

using std::cin;
using std::cout;
using std::complex;
using std::vector;

using cd = complex<double>;
const double PI = acos(-1.0);

// Рекурсивное FFT: invert=false -> прямое, invert=true -> обратное
void fft(vector<cd>& a, bool invert) {
    int n = (int)a.size();
    if (n == 1) return;

    // 1) Делим на чётные и нечётные коэффициенты
    vector<cd> a0(n / 2), a1(n / 2);
    for (int i = 0; i < n / 2; i++) {
        a0[i] = a[2 * i];
        a1[i] = a[2 * i + 1];
    }

    // 2) Рекурсивно считаем FFT для половинок
    fft(a0, invert);
    fft(a1, invert);

    // 3) Склеиваем результат бабочками w_n = exp(+-2πi/n)
    double ang = 2 * PI / n * (invert ? -1 : 1);
    cd wn(cos(ang), sin(ang));
    cd w(1);

    for (int k = 0; k < n / 2; k++) {
        cd t = w * a1[k];
        a[k] = a0[k] + t;
        a[k + n / 2] = a0[k] - t;
        w *= wn;
    }

    // 4) Нормировка для обратного преобразования
    if (invert) {
        for (cd & x : a) x /= 2.0;
    }
}

// Свёртка (умножение полиномов с целыми коэффициентами 0/1)
// Возвращает целые коэффициенты результата
static vector<long long> convolution01(const vector<int>& A, const vector<int>& B) {
    int n1 = (int)A.size();
    int n2 = (int)B.size();
    int size_ = n1 + n2 - 1;

    int n = 1;
    while (n < size_) n <<= 1;

    vector<cd> fa(n), fb(n);
    for (int i = 0; i < n1; i++) fa[i] = (double)A[i];
    for (int i = 0; i < n2; i++) fb[i] = (double)B[i];

    fft(fa, false);
    fft(fb, false);

    for (int i = 0; i < n; i++) fa[i] *= fb[i];

    fft(fa, true);

    vector<long long> res(size_);
    for (int i = 0; i < size_; i++) {
        // Округление к ближайшему целому
        res[i] = (long long) llround(fa[i].real());
    }
    return res;
}

int main() {

    int n;

    cout << "Введите количество степеней (элементов): ";
    cin >> n;

    vector<int> a_vals(n);
    cout << "Введите степени многочлена A (" << n << " чисел): ";
    int maxA = 0;
    for (int i = 0; i < n; i++) {
        cin >> a_vals[i];
        maxA = std::max(maxA, a_vals[i]);
    }

    vector<int> b_vals(n);
    cout << "Введите степени многочлена B (" << n << " чисел): ";
    int maxB = 0;
    for (int i = 0; i < n; i++) {
        cin >> b_vals[i];
        maxB = std::max(maxB, b_vals[i]);
    }

    vector<int> A(maxA + 1, 0), B(maxB + 1, 0);
    for (int x : a_vals) A[x] = 1;
    for (int y : b_vals) B[y] = 1;

    // cnt[s] = число пар таких, что a+b=s
    vector<long long> cnt = convolution01(A, B);

    cout << "\nРезультат (сумма -> количество способов):\n";
    for (int s = 0; s < (int)cnt.size(); s++) {
        if (cnt[s] > 0) {
            cout << s << " -> " << cnt[s] << "\n";
        }
    }

    return 0;
}