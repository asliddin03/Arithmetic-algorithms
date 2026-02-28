#include "gf2n_functions.h"
#include <stdexcept>
#include <algorithm>
#include <iostream>

string polynom_to_string_u64(u64 a) {
    if (a == 0) return "0";

    vector<string> terms;
    for (int i = 63; i >= 0; i--) {
        if ((a >> i) & 1ULL) {
            if (i == 0) terms.push_back("1");
            else if (i == 1) terms.push_back("x");
            else terms.push_back("x^" + std::to_string(i));
        }
    }

    string s;
    for (size_t i = 0; i < terms.size(); i++) {
        if (i) s += " + ";
        s += terms[i];
    }
    return s;
}

u64 polynom_from_degrees(const vector<int>& degrees) {
    u64 polynom = 0;
    for (int deg : degrees) {
        if (deg < 0 || deg > 63) throw std::runtime_error("degree out of range for u64");
        polynom ^= (1ULL << deg);
    }
    return polynom;
}

u128 polynom_mul_bin_u32(u64 a, u64 b) {
    u128 result = 0;
    while (b > 0) {
        if (b & 1ULL) {
            result ^= (u128)a;
        }
        a <<= 1;
        b >>= 1;
    }
    return result;
}

string u128_to_hex(u128 value) {
    if (value == 0) return "0x0";
    string result;
    static constexpr char HEX[] = "0123456789abcdef";
    while (value) {
        auto curr = static_cast<int>(value & 0xFULL);
        result.push_back(HEX[curr]);
        value >>= 4;
    }
    std::reverse(result.begin(), result.end());
    return "0x" + result;
}

int deg_u128(u128 value) {
    if (value == 0) return -1;
    for (int i = 127; i >= 0; i--) {
        if ((value >> i) & (u128)1) return i;
    }
    return -1;
}

u128 polynom_mod_u128(u128 a, u128 mod) {
    int dm = deg_u128(mod);
    if (dm < 0) throw std::runtime_error("mod is 0");
    while (true) {
        int da = deg_u128(a);
        if (da < dm) break;
        int shift = da - dm;
        a ^= (mod << shift);
    }
    return a;
}

u64 gf_mul(u64 a, u64 b, u128 mod, int n) {
    u128 prod = polynom_mul_bin_u32(a, b);
    u128 r = polynom_mod_u128(prod, mod);

    u64 mask = (n == 64) ? ~0ULL : ((1ULL << n) - 1ULL);
    return (u64)r & mask;
}

pair<u128, u128> polynom_div_mod_u128(u128 a, u128 b) {
    if (b == 0) throw std::runtime_error("division by zero");
    u128 q = 0;
    int db = deg_u128(b);
    while (a != 0) {
        int da = deg_u128(a);
        if (da < db) break;
        int shift = da - db;
        q ^= (u128)1 << shift;
        a ^= (b << shift);
    }
    return {q, a};
}

u128 polynom_mul_u128(u128 a, u128 b) {
    u128 result = 0;
    while (b) {
        if (b & 1) result ^= a;
        a <<= 1;
        b >>= 1;
    }
    return result;
}

EGCD polynom_ext_gcd(u128 a, u128 b) {
    u128 x0 = 1, y0 = 0;
    u128 x1 = 0, y1 = 1;

    while (b != 0) {
        auto [q, r] = polynom_div_mod_u128(a, b);

        a = b;
        b = r;

        u128 tx = x0 ^ polynom_mul_u128(q, x1);
        u128 ty = y0 ^ polynom_mul_u128(q, y1);

        x0 = x1; y0 = y1;
        x1 = tx; y1 = ty;
    }

    return {a, x0, y0};
}

u64 gf_inverse(u64 a, u128 mod, int n) {
    if (a == 0) throw std::runtime_error("0 has no inverse");

    EGCD egcd = polynom_ext_gcd((u128)a, mod);
    if (egcd.g != 1) {
        throw std::runtime_error("gcd != 1, inverse doesn't exist (mod may be reducible?)");
    }

    u128 xred = polynom_mod_u128(egcd.x, mod);
    u64 mask = (n == 64) ? ~0ULL : ((1ULL << n) - 1ULL);
    return (u64)xred & mask;
}