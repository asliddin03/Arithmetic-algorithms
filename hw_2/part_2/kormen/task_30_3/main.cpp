#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <iomanip>

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::complex;
using std::fixed;
using std::setprecision;

using cd = complex<double>;
static const double PI = std::acos(-1.0);

// Проверка: степень двойки или нет
static bool is_power_of_two(int x) {
    return x > 0 && (x & (x - 1)) == 0;
}

// Одномерное БПФ radix-2 (in-place)
// invert=false  -> прямое БПФ
// invert=true   -> обратное БПФ
static void fft1d(vector<cd>& a, bool invert) {
    int n = (int)a.size();

    // Перестановка бит-реверс
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        while (j & bit) {
            j ^= bit;
            bit >>= 1;
        }
        j ^= bit;
        if (i < j) {
            cd tmp = a[i];
            a[i] = a[j];
            a[j] = tmp;
        }
    }

    for (int len = 2; len <= n; len <<= 1) {
        double ang = 2.0 * PI / (double)len * (invert ? -1.0 : 1.0);
        cd wlen(std::cos(ang), std::sin(ang));

        for (int i = 0; i < n; i += len) {
            cd w(1.0, 0.0);
            for (int j = 0; j < len / 2; j++) {
                cd u = a[i + j];
                cd v = a[i + j + len / 2] * w;
                a[i + j] = u + v;
                a[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }

    if (invert) {
        for (int i = 0; i < n; i++) a[i] /= (double)n;
    }
}

// Страйды для "плоского" хранения d-мерного массива (row-major):
// strides[r] = произведение sizes[r+1] * ... * sizes[d-1]
static vector<long long> compute_strides(const vector<int>& sizes) {
    int d = (int)sizes.size();
    vector<long long> strides(d, 1);
    for (int r = d - 2; r >= 0; --r) {
        strides[r] = strides[r + 1] * (long long)sizes[r + 1];
    }
    return strides;
}

static void fft_along_axis(vector<cd>& data,
                           const vector<int>& sizes,
                           const vector<long long>& strides,
                           int axis,
                           bool invert) {
    int d = (int)sizes.size();
    int axis_len = sizes[axis];
    long long axis_stride = strides[axis];
    long long total = (long long)data.size();

    // Количество линий вдоль axis: total / axis_len
    long long num_lines = total / (long long)axis_len;

    vector<cd> line(axis_len);
    vector<int> coords(d, 0);

    // base_id пробегает все комбинации координат, кроме axis
    for (long long base_id = 0; base_id < num_lines; ++base_id) {
        long long tmp = base_id;

        // Разворачиваем tmp в координаты по измерениям, кроме axis
        for (int r = d - 1; r >= 0; --r) {
            if (r == axis) {
                coords[r] = 0;
                continue;
            }
            int dim = sizes[r];
            coords[r] = (int)(tmp % (long long)dim);
            tmp /= (long long)dim;
        }

        long long base = 0;
        for (int r = 0; r < d; ++r) {
            base += (long long)coords[r] * strides[r];
        }

        for (int t = 0; t < axis_len; ++t) {
            line[t] = data[base + (long long)t * axis_stride];
        }

        fft1d(line, invert);

        // Записываем обратно
        for (int t = 0; t < axis_len; ++t) {
            data[base + (long long)t * axis_stride] = line[t];
        }
    }
}

// d-мерное БПФ: последовательно считаем 1D БПФ по каждой оси
static void fft_nd(vector<cd>& data, const vector<int>& sizes, bool invert) {
    vector<long long> strides = compute_strides(sizes);
    int d = (int)sizes.size();

    for (int axis = 0; axis < d; ++axis) {
        fft_along_axis(data, sizes, strides, axis, invert);
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "=== d-мерное БПФ через последовательные 1D-БПФ ===\n";
    cout << "ВНИМАНИЕ: для этой реализации каждый размер n_i должен быть степенью 2 (radix-2 FFT).\n\n";

    cout << "Введите d (количество измерений): ";
    int d;
    cin >> d;

    if (d <= 0) {
        cerr << "Ошибка: d должно быть положительным.\n";
        return 1;
    }

    vector<int> sizes(d);
    long long n = 1;

    cout << "Введите размеры n1 n2 ... nd: ";
    for (int i = 0; i < d; ++i) {
        cin >> sizes[i];
        if (!is_power_of_two(sizes[i])) {
            cerr << "Ошибка: n" << (i + 1) << " = " << sizes[i]
                 << " не является степенью 2. Эта версия поддерживает только степени 2.\n";
            return 1;
        }
        n *= (long long)sizes[i];
    }

    cout << "Всего элементов n = n1*n2*...*nd = " << n << "\n";
    cout << "Введите " << n << " комплексных чисел (Re Im), по одному на строку.\n";
    cout << "Пример строки: 1.5 -2.0\n\n";

    vector<cd> data((size_t)n);
    for (long long i = 0; i < n; ++i) {
        double re, im;
        cin >> re >> im;
        data[(size_t)i] = cd(re, im);
    }

    cout << "Считаю прямое d-мерное БПФ...\n";
    fft_nd(data, sizes, /*invert=*/false);

    cout << "\n=== Результат (Re Im) ===\n";
    cout.setf(std::ios::fixed);
    cout << setprecision(10);

    for (long long i = 0; i < n; ++i) {
        cout << data[(size_t)i].real() << " " << data[(size_t)i].imag() << "\n";
    }

    return 0;
}