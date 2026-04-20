#pragma once

#include <cmath>
#include <complex>
#include <sstream>
#include <stdexcept>
#include <string>

using std::abs;
using std::complex;
using std::ostringstream;
using std::runtime_error;
using std::string;

class ModInt {
private:
    long long value_ = 0;
    long long modulus_ = 2;

    static long long normalize(long long value, long long mod) {
        if (mod <= 0) {
            throw runtime_error("Modulus must be positive.");
        }

        value %= mod;
        if (value < 0) {
            value += mod;
        }
        return value;
    }

    static long long gcdExtended(long long a, long long b, long long& x, long long& y) {
        if (b == 0) {
            x = 1;
            y = 0;
            return a;
        }

        long long x1 = 0;
        long long y1 = 0;
        long long g = gcdExtended(b, a % b, x1, y1);

        x = y1;
        y = x1 - (a / b) * y1;
        return g;
    }

public:
    ModInt() = default;

    ModInt(long long value, long long mod)
        : value_(normalize(value, mod)), modulus_(mod) {}

    long long value() const {
        return value_;
    }

    long long modulus() const {
        return modulus_;
    }

    bool isZero() const {
        return value_ == 0;
    }

    ModInt operator+(const ModInt& other) const {
        checkSameModulus(other);
        return ModInt(value_ + other.value_, modulus_);
    }

    ModInt operator-(const ModInt& other) const {
        checkSameModulus(other);
        return ModInt(value_ - other.value_, modulus_);
    }

    ModInt operator*(const ModInt& other) const {
        checkSameModulus(other);
        return ModInt(value_ * other.value_, modulus_);
    }

    ModInt inverse() const {
        if (isZero()) {
            throw runtime_error("Zero has no inverse in Zn.");
        }

        long long x = 0;
        long long y = 0;
        long long g = gcdExtended(value_, modulus_, x, y);

        if (g != 1) {
            throw runtime_error("Element is not invertible in Zn.");
        }

        return ModInt(x, modulus_);
    }

    ModInt operator/(const ModInt& other) const {
        checkSameModulus(other);
        return (*this) * other.inverse();
    }

    bool operator==(const ModInt& other) const {
        checkSameModulus(other);
        return value_ == other.value_;
    }

    bool operator!=(const ModInt& other) const {
        return !(*this == other);
    }

    string toString() const {
        return std::to_string(value_);
    }

    void checkSameModulus(const ModInt& other) const {
        if (modulus_ != other.modulus_) {
            throw runtime_error("Different moduli in Zn coefficients.");
        }
    }
};

struct IntegerOps {
    using Coef = long long;

    static Coef zero() {
        return 0;
    }

    static Coef one() {
        return 1;
    }

    static bool isZero(Coef x) {
        return x == 0;
    }

    static bool equal(Coef a, Coef b) {
        return a == b;
    }

    static Coef add(Coef a, Coef b) {
        return a + b;
    }

    static Coef sub(Coef a, Coef b) {
        return a - b;
    }

    static Coef mul(Coef a, Coef b) {
        return a * b;
    }

    static Coef divide(Coef a, Coef b) {
        if (b == 0) {
            throw runtime_error("Division by zero.");
        }
        if (a % b != 0) {
            throw runtime_error("Non-exact integer division.");
        }
        return a / b;
    }

    static string toString(Coef x) {
        return std::to_string(x);
    }
};

struct ModOps {
    using Coef = ModInt;

    static Coef zeroLike(const Coef& sample) {
        return Coef(0, sample.modulus());
    }

    static Coef oneLike(const Coef& sample) {
        return Coef(1, sample.modulus());
    }

    static bool isZero(const Coef& x) {
        return x.isZero();
    }

    static bool equal(const Coef& a, const Coef& b) {
        return a == b;
    }

    static Coef add(const Coef& a, const Coef& b) {
        return a + b;
    }

    static Coef sub(const Coef& a, const Coef& b) {
        return a - b;
    }

    static Coef mul(const Coef& a, const Coef& b) {
        return a * b;
    }

    static Coef divide(const Coef& a, const Coef& b) {
        return a / b;
    }

    static string toString(const Coef& x) {
        return x.toString();
    }
};

struct RealOps {
    using Coef = double;

    static constexpr double EPS = 1e-9;

    static Coef zero() {
        return 0.0;
    }

    static Coef one() {
        return 1.0;
    }

    static bool isZero(Coef x) {
        return abs(x) < EPS;
    }

    static bool equal(Coef a, Coef b) {
        return abs(a - b) < EPS;
    }

    static Coef add(Coef a, Coef b) {
        return a + b;
    }

    static Coef sub(Coef a, Coef b) {
        return a - b;
    }

    static Coef mul(Coef a, Coef b) {
        return a * b;
    }

    static Coef divide(Coef a, Coef b) {
        if (abs(b) < EPS) {
            throw runtime_error("Division by zero.");
        }
        return a / b;
    }

    static string toString(Coef x) {
        ostringstream out;
        out << x;
        return out.str();
    }
};

struct ComplexOps {
    using Coef = complex<double>;

    static constexpr double EPS = 1e-9;

    static Coef zero() {
        return Coef(0.0, 0.0);
    }

    static Coef one() {
        return Coef(1.0, 0.0);
    }

    static bool isZero(const Coef& x) {
        return abs(x) < EPS;
    }

    static bool equal(const Coef& a, const Coef& b) {
        return abs(a - b) < EPS;
    }

    static Coef add(const Coef& a, const Coef& b) {
        return a + b;
    }

    static Coef sub(const Coef& a, const Coef& b) {
        return a - b;
    }

    static Coef mul(const Coef& a, const Coef& b) {
        return a * b;
    }

    static Coef divide(const Coef& a, const Coef& b) {
        if (abs(b) < EPS) {
            throw runtime_error("Division by zero.");
        }
        return a / b;
    }

    static string toString(const Coef& x) {
        ostringstream out;

        double re = x.real();
        double im = x.imag();

        bool reZero = abs(re) < EPS;
        bool imZero = abs(im) < EPS;

        if (reZero && imZero) {
            return "0";
        }

        if (!reZero) {
            out << re;
        }

        if (!imZero) {
            if (!reZero) {
                out << (im >= 0 ? "+" : "");
            }

            if (abs(im - 1.0) < EPS) {
                out << "i";
            } else if (abs(im + 1.0) < EPS) {
                out << "-i";
            } else {
                out << im << "i";
            }
        }

        return out.str();
    }
};