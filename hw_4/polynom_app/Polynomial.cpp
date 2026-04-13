#include "Polynomial.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <numeric>

Polynomial::Polynomial(const vector<std::string>& varNames)
    : vars(varNames) {}

int Polynomial::varCount() const {
    return static_cast<int>(vars.size());
}

const vector<std::string>& Polynomial::variableNames() const {
    return vars;
}

void Polynomial::setVariableNames(const vector<std::string>& varNames) {
    vars = varNames;
    clear();
}

void Polynomial::clear() {
    data.clear();
}

void Polynomial::checkExponents(const vector<int>& exponents) const {
    if (static_cast<int>(exponents.size()) != varCount()) {
        throw std::runtime_error("Wrong number of exponents.");
    }

    for (int e : exponents) {
        if (e < 0) {
            throw std::runtime_error("Negative exponents are not allowed.");
        }
    }
}

bool Polynomial::sameMonomial(const vector<int>& a, const vector<int>& b) {
    return a == b;
}

void Polynomial::combineLikeTerms() {
    vector<Term> result;

    for (const auto& t : data) {
        if (t.coef == 0) {
            continue;
        }

        bool found = false;
        for (auto& r : result) {
            if (sameMonomial(r.exponents, t.exponents)) {
                r.coef += t.coef;
                found = true;
                break;
            }
        }

        if (!found) {
            result.push_back(t);
        }
    }

    data.clear();
    for (const auto& t : result) {
        if (t.coef != 0) {
            data.push_back(t);
        }
    }
}

void Polynomial::addMonomial(Coef coef, const vector<int>& exponents) {
    checkExponents(exponents);
    if (coef == 0) {
        return;
    }

    for (auto& t : data) {
        if (sameMonomial(t.exponents, exponents)) {
            t.coef += coef;
            combineLikeTerms();
            return;
        }
    }

    data.push_back({coef, exponents});
}

void Polynomial::setMonomial(Coef coef, const vector<int>& exponents) {
    checkExponents(exponents);

    for (auto& t : data) {
        if (sameMonomial(t.exponents, exponents)) {
            t.coef = coef;
            combineLikeTerms();
            return;
        }
    }

    if (coef != 0) {
        data.push_back({coef, exponents});
    }
}

const vector<Term>& Polynomial::terms() const {
    return data;
}

bool Polynomial::isZero() const {
    return data.empty();
}

Polynomial& Polynomial::operator+=(const Polynomial& other) {
    if (vars != other.vars) {
        throw std::runtime_error("Polynomials use different variables.");
    }

    for (const auto& t : other.data) {
        addMonomial(t.coef, t.exponents);
    }

    return *this;
}

Polynomial& Polynomial::operator-=(const Polynomial& other) {
    if (vars != other.vars) {
        throw std::runtime_error("Polynomials use different variables.");
    }

    for (const auto& t : other.data) {
        addMonomial(-t.coef, t.exponents);
    }

    return *this;
}

Polynomial& Polynomial::operator*=(const Polynomial& other) {
    if (vars != other.vars) {
        throw std::runtime_error("Polynomials use different variables.");
    }

    Polynomial result(vars);

    for (const auto& a : data) {
        for (const auto& b : other.data) {
            vector<int> exponents(varCount(), 0);
            for (int i = 0; i < varCount(); ++i) {
                exponents[i] = a.exponents[i] + b.exponents[i];
            }
            result.addMonomial(a.coef * b.coef, exponents);
        }
    }

    *this = std::move(result);
    return *this;
}

Polynomial Polynomial::operator+(const Polynomial& other) const {
    Polynomial result = *this;
    result += other;
    return result;
}

Polynomial Polynomial::operator-(const Polynomial& other) const {
    Polynomial result = *this;
    result -= other;
    return result;
}

Polynomial Polynomial::operator*(const Polynomial& other) const {
    Polynomial result = *this;
    result *= other;
    return result;
}

bool Polynomial::operator==(const Polynomial& other) const {
    if (vars != other.vars) {
        throw std::runtime_error("Polynomials use different variables.");
    }

    Polynomial a = *this;
    Polynomial b = other;
    a.combineLikeTerms();
    b.combineLikeTerms();

    if (a.data.size() != b.data.size()) {
        return false;
    }

    auto cmp = [](const Term& x, const Term& y) {
        return x.exponents < y.exponents;
    };

    std::sort(a.data.begin(), a.data.end(), cmp);
    std::sort(b.data.begin(), b.data.end(), cmp);

    for (size_t i = 0; i < a.data.size(); ++i) {
        if (a.data[i].coef != b.data[i].coef) {
            return false;
        }
        if (a.data[i].exponents != b.data[i].exponents) {
            return false;
        }
    }

    return true;
}

bool Polynomial::operator!=(const Polynomial& other) const {
    return !(*this == other);
}

vector<vector<int>> Polynomial::support() const {
    vector<vector<int>> result;
    for (const auto& t : data) {
        result.push_back(t.exponents);
    }
    return result;
}

Coef fastPow(Coef a, int e) {
    if (e < 0) {
        throw std::runtime_error("Exponent must be non-negative.");
    }

    Coef result = 1;
    while (e > 0) {
        if (e & 1) {
            result *= a;
        }
        a *= a;
        e >>= 1;
    }
    return result;
}

Coef Polynomial::evaluateAt(const vector<Coef>& point) const {
    if (static_cast<int>(point.size()) != varCount()) {
        throw std::runtime_error("Point size does not match variable count.");
    }

    Coef result = 0;

    for (const auto& t : data) {
        Coef value = t.coef;
        for (int i = 0; i < varCount(); ++i) {
            value *= fastPow(point[i], t.exponents[i]);
        }
        result += value;
    }

    return result;
}

int totalDegree(const vector<int>& exponents) {
    int s = 0;
    for (int x : exponents) {
        s += x;
    }
    return s;
}

int Polynomial::homeDegree() const {
    if (data.empty()) {
        return 0;
    }

    int d = totalDegree(data.front().exponents);
    for (const auto& t : data) {
        if (totalDegree(t.exponents) != d) {
            return -1;
        }
    }

    return d;
}

std::pair<Polynomial, Polynomial> Polynomial::splitByDegree(int degree) const {
    Polynomial h(vars);
    Polynomial g(vars);

    for (const auto& t : data) {
        if (totalDegree(t.exponents) == degree) {
            h.addMonomial(t.coef, t.exponents);
        } else {
            g.addMonomial(t.coef, t.exponents);
        }
    }

    return {h, g};
}

bool Polynomial::isGreaterLex(
    const vector<int>& a,
    const vector<int>& b
) {
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) {
            return a[i] > b[i];
        }
    }
    return false;
}

bool Polynomial::isGreaterGrevLexTie(
    const vector<int>& a,
    const vector<int>& b
) {
    for (int i = static_cast<int>(a.size()) - 1; i >= 0; --i) {
        if (a[i] != b[i]) {
            return a[i] < b[i];
        }
    }
    return false;
}

int Polynomial::compareTotalDegree(
    const vector<int>& a,
    const vector<int>& b
) {
    int da = totalDegree(a);
    int db = totalDegree(b);

    if (da > db) return 1;
    if (da < db) return -1;
    return 0;
}

bool Polynomial::isGreaterMonomial(
    const vector<int>& a,
    const vector<int>& b,
    MonomialOrder order
) {
    if (a.size() != b.size()) {
        throw std::runtime_error("Monomials have different dimension.");
    }

    switch (order) {
        case MonomialOrder::Lex:
            return isGreaterLex(a, b);

        case MonomialOrder::GrLex: {
            int cmp = compareTotalDegree(a, b);
            if (cmp != 0) {
                return cmp > 0;
            }
            return isGreaterLex(a, b);
        }

        case MonomialOrder::GrevLex: {
            int cmp = compareTotalDegree(a, b);
            if (cmp != 0) {
                return cmp > 0;
            }
            return isGreaterGrevLexTie(a, b);
        }
    }

    return false;
}

LeadingData Polynomial::leadingData(MonomialOrder order) const {
    LeadingData result;
    if (data.empty()) {
        return result;
    }

    result.coef = data[0].coef;
    result.exponents = data[0].exponents;
    result.exists = true;

    for (size_t i = 1; i < data.size(); ++i) {
        if (isGreaterMonomial(data[i].exponents, result.exponents, order)) {
            result.coef = data[i].coef;
            result.exponents = data[i].exponents;
        }
    }

    return result;
}

vector<int> Polynomial::leadingMonomial(MonomialOrder order) const {
    LeadingData d = leadingData(order);
    if (!d.exists) {
        throw std::runtime_error("Zero polynomial has no leading monomial.");
    }
    return d.exponents;
}

Coef Polynomial::leadingCoefficient(MonomialOrder order) const {
    LeadingData d = leadingData(order);
    if (!d.exists) {
        throw std::runtime_error("Zero polynomial has no leading coefficient.");
    }
    return d.coef;
}

Term Polynomial::leadingTerm(MonomialOrder order) const {
    LeadingData d = leadingData(order);
    if (!d.exists) {
        throw std::runtime_error("Zero polynomial has no leading term.");
    }
    return {d.coef, d.exponents};
}

vector<int> Polynomial::multiDegree(MonomialOrder order) const {
    return leadingMonomial(order);
}

std::string Polynomial::monomialOnlyToString(const vector<int>& exponents) const {
    checkExponents(exponents);

    bool hasVars = false;
    for (int e : exponents) {
        if (e != 0) {
            hasVars = true;
            break;
        }
    }

    if (!hasVars) {
        return "1";
    }

    std::ostringstream out;
    for (int i = 0; i < varCount(); ++i) {
        if (exponents[i] == 0) {
            continue;
        }
        out << vars[i];
        if (exponents[i] != 1) {
            out << "^" << exponents[i];
        }
    }
    return out.str();
}

std::string Polynomial::monomialToString(const Term& t) const {
    bool hasVars = false;
    for (int e : t.exponents) {
        if (e != 0) {
            hasVars = true;
            break;
        }
    }

    if (!hasVars) {
        return std::to_string(t.coef);
    }

    std::ostringstream out;

    if (t.coef == -1) {
        out << "-";
    } else if (t.coef != 1) {
        out << t.coef;
    }

    for (int i = 0; i < varCount(); ++i) {
        if (t.exponents[i] == 0) {
            continue;
        }
        out << vars[i];
        if (t.exponents[i] != 1) {
            out << "^" << t.exponents[i];
        }
    }

    return out.str();
}

std::string Polynomial::toString() const {
    return toString(MonomialOrder::Lex);
}

std::string Polynomial::toString(MonomialOrder order) const {
    if (data.empty()) {
        return "0";
    }

    vector<Term> ts = data;
    std::sort(ts.begin(), ts.end(), [order](const Term& a, const Term& b) {
        return Polynomial::isGreaterMonomial(a.exponents, b.exponents, order);
    });

    std::ostringstream out;
    bool first = true;

    for (const auto& t : ts) {
        if (t.coef == 0) {
            continue;
        }

        std::string piece = monomialToString(t);

        if (first) {
            out << piece;
            first = false;
        } else if (t.coef < 0) {
            out << " - ";
            if (!piece.empty() && piece[0] == '-') {
                out << piece.substr(1);
            } else {
                out << piece;
            }
        } else {
            out << " + " << piece;
        }
    }

    return first ? "0" : out.str();
}

vector<int> Polynomial::lcmMonomial(const vector<int>& a, const vector<int>& b) const {
    if (a.size() != b.size()) {
        throw std::runtime_error("Monomials have different dimension.");
    }

    vector<int> result(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        result[i] = std::max(a[i], b[i]);
    }
    return result;
}

vector<int> Polynomial::subtractMonomial(const vector<int>& a, const vector<int>& b) const {
    if (a.size() != b.size()) {
        throw std::runtime_error("Monomials have different dimension.");
    }

    vector<int> result(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] < b[i]) {
            throw std::runtime_error("Negative exponent in monomial subtraction.");
        }
        result[i] = a[i] - b[i];
    }
    return result;
}

bool Polynomial::dividesMonomial(const vector<int>& a, const vector<int>& b) const {
    if (a.size() != b.size()) {
        throw std::runtime_error("Monomials have different dimension.");
    }

    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] > b[i]) {
            return false;
        }
    }
    return true;
}

Coef Polynomial::exactDivide(Coef a, Coef b) const {
    if (b == 0) {
        throw std::runtime_error("Division by zero.");
    }
    if (a % b != 0) {
        throw std::runtime_error("Non-exact integer division in polynomial operation.");
    }
    return a / b;
}

vector<int> Polynomial::multiDegreePair(const Polynomial& other, MonomialOrder order) const {
    if (vars != other.vars) {
        throw std::runtime_error("Polynomials use different variables.");
    }
    if (isZero() || other.isZero()) {
        throw std::runtime_error("multideg(f, g) is undefined for zero polynomial.");
    }

    const vector<int> lmThis = leadingMonomial(order);
    const vector<int> lmOther = other.leadingMonomial(order);

    return lcmMonomial(lmThis, lmOther);
}

Polynomial Polynomial::multiplyByMonomial(Coef coef, const vector<int>& exponents) const {
    checkExponents(exponents);

    if (coef == 0 || isZero()) {
        return Polynomial(vars);
    }

    Polynomial result(vars);
    result.data.reserve(data.size());

    for (const auto& t : data) {
        vector<int> newExp(varCount());
        for (int i = 0; i < varCount(); ++i) {
            newExp[i] = t.exponents[i] + exponents[i];
        }
        result.data.push_back({t.coef * coef, std::move(newExp)});
    }

    return result;
}

Polynomial Polynomial::sPolynomial(const Polynomial& other, MonomialOrder order) const {
    if (vars != other.vars) {
        throw std::runtime_error("Polynomials use different variables.");
    }
    if (isZero() || other.isZero()) {
        throw std::runtime_error("S-polynomial is undefined for zero polynomial.");
    }

    const Term ltF = leadingTerm(order);
    const Term ltG = other.leadingTerm(order);

    const vector<int> lcmExp = lcmMonomial(ltF.exponents, ltG.exponents);

    const vector<int> mulFExp = subtractMonomial(lcmExp, ltF.exponents);
    const vector<int> mulGExp = subtractMonomial(lcmExp, ltG.exponents);

    const Coef gcdCoef = std::gcd(ltF.coef, ltG.coef);
    const Coef lcmCoef = (ltF.coef / gcdCoef) * ltG.coef;

    const Coef mulFCoef = exactDivide(lcmCoef, ltF.coef);
    const Coef mulGCoef = exactDivide(lcmCoef, ltG.coef);

    Polynomial left = multiplyByMonomial(mulFCoef, mulFExp);
    Polynomial right = other.multiplyByMonomial(mulGCoef, mulGExp);

    return left - right;
}

Polynomial Polynomial::remainderOnDivision(const vector<Polynomial>& basis, MonomialOrder order) const {
    for (const auto& g : basis) {
        if (vars != g.variableNames()) {
            throw std::runtime_error("Basis polynomials use different variables.");
        }
    }

    Polynomial p = *this;
    Polynomial r(vars);

    while (!p.isZero()) {
        const Term ltP = p.leadingTerm(order);
        bool reduced = false;

        for (const auto& g : basis) {
            if (g.isZero()) {
                continue;
            }

            const Term ltG = g.leadingTerm(order);

            if (!dividesMonomial(ltG.exponents, ltP.exponents)) {
                continue;
            }

            if (ltG.coef == 0) {
                continue;
            }

            if (ltP.coef % ltG.coef != 0) {
                continue;
            }

            const Coef coefQuot = exactDivide(ltP.coef, ltG.coef);
            const std::vector<int> expQuot = subtractMonomial(ltP.exponents, ltG.exponents);

            p -= g.multiplyByMonomial(coefQuot, expQuot);
            reduced = true;
            break;
        }

        if (!reduced) {
            r.addMonomial(ltP.coef, ltP.exponents);
            p.addMonomial(-ltP.coef, ltP.exponents);
        }
    }

    return r;
}

bool Polynomial::isGroebnerBasis(const vector<Polynomial>& basis, MonomialOrder order) {
    if (basis.empty()) {
        return true;
    }

    const std::vector<std::string>& vars = basis.front().variableNames();

    for (const auto& g : basis) {
        if (g.variableNames() != vars) {
            throw std::runtime_error("Basis polynomials use different variables.");
        }
    }

    for (size_t i = 0; i < basis.size(); ++i) {
        if (basis[i].isZero()) {
            continue;
        }

        for (size_t j = i + 1; j < basis.size(); ++j) {
            if (basis[j].isZero()) {
                continue;
            }

            const Polynomial s = basis[i].sPolynomial(basis[j], order);
            const Polynomial rem = s.remainderOnDivision(basis, order);

            if (!rem.isZero()) {
                return false;
            }
        }
    }

    return true;
}