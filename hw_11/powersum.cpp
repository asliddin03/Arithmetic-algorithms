#include "powersum.h"

long double fastPower(long double base, int power, long long& operations) {
    long double result = 1.0L;

    while (power > 0) {
        operations++;

        if (power % 2 == 1) {
            result *= base;
            operations++;
        }

        base *= base;
        power /= 2;
        operations += 2;
    }

    return result;
}

Result sumDirect(int n, int p) {
    long double sum = 0.0L;
    long long operations = 0;

    for (int k = 1; k <= n; k++) {
        long double value = fastPower(k, p, operations);
        sum += value;
        operations++;
    }

    return {sum, operations};
}

Result sumAsymptotic(int n, int p) {
    long long operations = 0;

    long double term1 = fastPower(n, p + 1, operations) / (p + 1);
    operations++;

    long double term2 = fastPower(n, p, operations) / 2.0L;
    operations++;

    long double term3 = 0.0L;

    if (p >= 1) {
        term3 = (p / 12.0L) * fastPower(n, p - 1, operations);
        operations += 2;
    }

    long double result = term1 + term2 + term3;
    operations += 2;

    return {result, operations};
}

long double binomial(int n, int k, long long& operations) {
    if (k < 0 || k > n) {
        return 0.0L;
    }

    if (k > n - k) {
        k = n - k;
    }

    long double result = 1.0L;

    for (int i = 1; i <= k; i++) {
        result *= (n - k + i);
        result /= i;
        operations += 2;
    }

    return result;
}

std::vector<long double> bernoulliNumbers(int p, long long& operations) {
    std::vector<long double> B(p + 1, 0.0L);

    B[0] = 1.0L;

    for (int j = 1; j <= p; j++) {
        long double sum = 0.0L;

        for (int k = 1; k <= j; k++) {
            long double c = binomial(j + 1, k + 1, operations);
            sum += c * B[j - k];
            operations += 2;
        }

        B[j] = -sum / (j + 1);
        operations += 2;
    }

    return B;
}

Result sumExactFormula(int n, int p) {
    long long operations = 0;

    std::vector<long double> B = bernoulliNumbers(p, operations);

    long double sum = 0.0L;

    for (int j = 0; j <= p; j++) {
        long double c = binomial(p + 1, j, operations);
        long double power = fastPower(n, p + 1 - j, operations);

        sum += c * B[j] * power;
        operations += 3;
    }

    sum /= (p + 1);
    operations++;

    return {sum, operations};
}