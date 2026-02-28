#pragma once
#include <string>
#include <vector>
#include <utility>

using u64  = uint64_t;
using u128 = unsigned __int128;

using std::string;
using std::vector;
using std::pair;

// Пункт 1
string polynom_to_string_u64(u64 a);
u64 polynom_from_degrees(const vector<int>& degrees);

// Служебные
string u128_to_hex(u128 value);
int deg_u128(u128 value);

// Пункт 2
u128 polynom_mul_bin_u32(u64 a, u64 b);

// Остаток/деление/умножение u128 (нужно для 3/4/5)
u128 polynom_mod_u128(u128 a, u128 mod);
pair<u128, u128> polynom_div_mod_u128(u128 a, u128 b);
u128 polynom_mul_u128(u128 a, u128 b);

// Пункт 3
u64 gf_mul(u64 a, u64 b, u128 mod, int n);

// Пункт 4
struct EGCD {
    u128 g; // gcd(a,b)
    u128 x; // коэффициент при a
    u128 y; // коэффициент при b
};
EGCD polynom_ext_gcd(u128 a, u128 b);

// Пункт 5
u64 gf_inverse(u64 a, u128 mod, int n);