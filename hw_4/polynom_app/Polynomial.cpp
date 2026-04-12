#include "Polynomial.h"

#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <stdexcept>

namespace {

Coef mulMod(Coef a, Coef b, Coef mod) {
    return static_cast<Coef>((__int128)a * (__int128)b % (__int128)mod);
}

} // namespace

PolynomialTrie::PolynomialTrie()
    : root(std::make_unique<Node>()),
      domain(CoefDomain::Zk),
      modulus(2) {}

PolynomialTrie::PolynomialTrie(const std::vector<std::string>& varNames, CoefDomain d, Coef mod)
    : root(std::make_unique<Node>()),
      vars(varNames),
      domain(d),
      modulus(mod) {
    if (domain == CoefDomain::Zk) {
        checkModulus(modulus);
    }
}

std::unique_ptr<PolynomialTrie::Node> PolynomialTrie::cloneNode(const Node* node) const {
    if (!node) {
        return nullptr;
    }

    auto cloned = std::make_unique<Node>();
    cloned->coef = node->coef;
    cloned->hasCoef = node->hasCoef;

    for (const auto& [exp, child] : node->children) {
        cloned->children[exp] = cloneNode(child.get());
    }

    return cloned;
}

PolynomialTrie::PolynomialTrie(const PolynomialTrie& other)
    : root(cloneNode(other.root.get())),
      vars(other.vars),
      domain(other.domain),
      modulus(other.modulus) {}

PolynomialTrie& PolynomialTrie::operator=(const PolynomialTrie& other) {
    if (this == &other) {
        return *this;
    }

    root = cloneNode(other.root.get());
    vars = other.vars;
    domain = other.domain;
    modulus = other.modulus;
    return *this;
}

void PolynomialTrie::checkModulus(Coef mod) const {
    if (mod <= 0) {
        throw std::runtime_error("Modulus must be positive.");
    }
}

void PolynomialTrie::checkExponents(const std::vector<int>& exponents) const {
    if (static_cast<int>(exponents.size()) != varCount()) {
        throw std::runtime_error("Wrong number of exponents.");
    }

    for (int exp : exponents) {
        if (exp < 0) {
            throw std::runtime_error("Negative exponents are not allowed.");
        }
    }
}

void PolynomialTrie::checkCompatible(const PolynomialTrie& other) const {
    if (vars != other.vars) {
        throw std::runtime_error("Polynomials use different variables.");
    }
    if (domain != other.domain) {
        throw std::runtime_error("Polynomials use different coefficient domains.");
    }
    if (domain == CoefDomain::Zk && modulus != other.modulus) {
        throw std::runtime_error("Polynomials use different modulus.");
    }
}

int PolynomialTrie::varCount() const {
    return static_cast<int>(vars.size());
}

const std::vector<std::string>& PolynomialTrie::variableNames() const {
    return vars;
}

CoefDomain PolynomialTrie::getDomain() const {
    return domain;
}

Coef PolynomialTrie::getModulus() const {
    return modulus;
}

void PolynomialTrie::setVariableNames(const std::vector<std::string>& varNames) {
    vars = varNames;
    clear();
}

void PolynomialTrie::setDomain(CoefDomain newDomain, Coef mod) {
    if (newDomain == CoefDomain::Zk) {
        checkModulus(mod);
    }

    domain = newDomain;
    modulus = mod;
    normalize();
}

void PolynomialTrie::setModulus(Coef mod) {
    checkModulus(mod);
    modulus = mod;

    if (domain == CoefDomain::Zk) {
        normalize();
    }
}

void PolynomialTrie::clear() {
    root = std::make_unique<Node>();
}

Coef PolynomialTrie::normalizeCoef(Coef value) const {
    if (domain == CoefDomain::Z) {
        return value;
    }

    value %= modulus;
    if (value < 0) {
        value += modulus;
    }
    return value;
}

Coef PolynomialTrie::displayCoef(Coef value) const {
    value = normalizeCoef(value);

    if (domain == CoefDomain::Z) {
        return value;
    }

    Coef alt = value - modulus;
    return (std::llabs(alt) < std::llabs(value)) ? alt : value;
}

void PolynomialTrie::addMonomial(Coef coef, const std::vector<int>& exponents) {
    checkExponents(exponents);

    coef = normalizeCoef(coef);
    if (coef == 0) {
        return;
    }

    Node* current = root.get();

    for (int level = 0; level < varCount(); ++level) {
        int exp = exponents[level];
        auto it = current->children.find(exp);
        if (it == current->children.end()) {
            auto inserted = current->children.emplace(exp, std::make_unique<Node>());
            current = inserted.first->second.get();
        } else {
            current = it->second.get();
        }
    }

    if (current->hasCoef) {
        current->coef = normalizeCoef(current->coef + coef);
        if (current->coef == 0) {
            current->coef = 0;
            current->hasCoef = false;
        }
    } else {
        current->coef = coef;
        current->hasCoef = true;
    }
}

void PolynomialTrie::setMonomial(Coef coef, const std::vector<int>& exponents) {
    checkExponents(exponents);

    Node* current = root.get();

    for (int level = 0; level < varCount(); ++level) {
        int exp = exponents[level];
        auto it = current->children.find(exp);
        if (it == current->children.end()) {
            auto inserted = current->children.emplace(exp, std::make_unique<Node>());
            current = inserted.first->second.get();
        } else {
            current = it->second.get();
        }
    }

    coef = normalizeCoef(coef);
    if (coef == 0) {
        current->coef = 0;
        current->hasCoef = false;
    } else {
        current->coef = coef;
        current->hasCoef = true;
    }
}

void PolynomialTrie::collectTermsDFS(
    const Node* node,
    int level,
    std::vector<int>& path,
    std::vector<Term>& result
) const {
    if (!node) {
        return;
    }

    if (level == varCount()) {
        if (node->hasCoef && node->coef != 0) {
            result.push_back(Term{node->coef, path});
        }
        return;
    }

    for (const auto& [exp, child] : node->children) {
        path[level] = exp;
        collectTermsDFS(child.get(), level + 1, path, result);
    }
}

std::vector<Term> PolynomialTrie::terms() const {
    std::vector<Term> result;
    std::vector<int> path(varCount(), 0);
    collectTermsDFS(root.get(), 0, path, result);
    return result;
}

void PolynomialTrie::normalize() {
    const std::vector<Term> ts = terms();
    clear();

    for (const auto& term : ts) {
        addMonomial(term.coef, term.exponents);
    }
}

bool PolynomialTrie::isZero() const {
    return terms().empty();
}

PolynomialTrie& PolynomialTrie::operator+=(const PolynomialTrie& other) {
    checkCompatible(other);

    for (const auto& term : other.terms()) {
        addMonomial(term.coef, term.exponents);
    }

    return *this;
}

PolynomialTrie& PolynomialTrie::operator-=(const PolynomialTrie& other) {
    checkCompatible(other);

    for (const auto& term : other.terms()) {
        addMonomial(-term.coef, term.exponents);
    }

    return *this;
}

PolynomialTrie& PolynomialTrie::operator*=(const PolynomialTrie& other) {
    checkCompatible(other);

    const std::vector<Term> left = terms();
    const std::vector<Term> right = other.terms();

    PolynomialTrie result(vars, domain, modulus);

    for (const auto& a : left) {
        for (const auto& b : right) {
            std::vector<int> exponents(varCount(), 0);
            for (int i = 0; i < varCount(); ++i) {
                exponents[i] = a.exponents[i] + b.exponents[i];
            }

            Coef coef;
            if (domain == CoefDomain::Zk) {
                coef = mulMod(normalizeCoef(a.coef), normalizeCoef(b.coef), modulus);
                coef = normalizeCoef(coef);
            } else {
                coef = a.coef * b.coef;
            }

            result.addMonomial(coef, exponents);
        }
    }

    *this = std::move(result);
    return *this;
}

PolynomialTrie PolynomialTrie::operator+(const PolynomialTrie& other) const {
    PolynomialTrie result = *this;
    result += other;
    return result;
}

PolynomialTrie PolynomialTrie::operator-(const PolynomialTrie& other) const {
    PolynomialTrie result = *this;
    result -= other;
    return result;
}

PolynomialTrie PolynomialTrie::operator*(const PolynomialTrie& other) const {
    PolynomialTrie result = *this;
    result *= other;
    return result;
}

bool PolynomialTrie::operator==(const PolynomialTrie& other) const {
    checkCompatible(other);

    const std::vector<Term> a = terms();
    const std::vector<Term> b = other.terms();

    if (a.size() != b.size()) {
        return false;
    }

    for (std::size_t i = 0; i < a.size(); ++i) {
        if (normalizeCoef(a[i].coef) != normalizeCoef(b[i].coef)) {
            return false;
        }
        if (a[i].exponents != b[i].exponents) {
            return false;
        }
    }

    return true;
}

bool PolynomialTrie::operator!=(const PolynomialTrie& other) const {
    return !(*this == other);
}

std::vector<std::vector<int>> PolynomialTrie::support() const {
    std::vector<std::vector<int>> result;
    for (const auto& term : terms()) {
        result.push_back(term.exponents);
    }
    return result;
}

void PolynomialTrie::collectMaxExponentsDFS(
    const Node* node,
    int level,
    std::vector<int>& maxExp
) const {
    if (!node || level == varCount()) {
        return;
    }

    for (const auto& [exp, child] : node->children) {
        if (exp > maxExp[level]) {
            maxExp[level] = exp;
        }
        collectMaxExponentsDFS(child.get(), level + 1, maxExp);
    }
}

Coef PolynomialTrie::evaluateDFS(
    const Node* node,
    int level,
    Coef currentValue,
    const std::vector<std::vector<Coef>>& powers
) const {
    if (!node) {
        return 0;
    }

    if (level == varCount()) {
        if (!node->hasCoef) {
            return 0;
        }

        if (domain == CoefDomain::Zk) {
            return mulMod(node->coef, currentValue, modulus);
        }
        return node->coef * currentValue;
    }

    Coef result = 0;

    for (const auto& [exp, child] : node->children) {
        Coef nextValue;
        if (domain == CoefDomain::Zk) {
            nextValue = mulMod(currentValue, powers[level][exp], modulus);
            result += evaluateDFS(child.get(), level + 1, nextValue, powers);
            result = normalizeCoef(result);
        } else {
            nextValue = currentValue * powers[level][exp];
            result += evaluateDFS(child.get(), level + 1, nextValue, powers);
        }
    }

    return normalizeCoef(result);
}

Coef PolynomialTrie::evaluateAt(const std::vector<Coef>& point) const {
    if (static_cast<int>(point.size()) != varCount()) {
        throw std::runtime_error("Point size does not match variable count.");
    }

    if (isZero()) {
        return 0;
    }

    if (domain == CoefDomain::Zk && modulus == 1) {
        return 0;
    }

    std::vector<int> maxExp(varCount(), 0);
    collectMaxExponentsDFS(root.get(), 0, maxExp);

    std::vector<std::vector<Coef>> powers(varCount());

    for (int i = 0; i < varCount(); ++i) {
        Coef base = normalizeCoef(point[i]);
        powers[i].resize(maxExp[i] + 1);
        powers[i][0] = (domain == CoefDomain::Zk) ? (1 % modulus) : 1;

        for (int e = 1; e <= maxExp[i]; ++e) {
            if (domain == CoefDomain::Zk) {
                powers[i][e] = mulMod(powers[i][e - 1], base, modulus);
            } else {
                powers[i][e] = powers[i][e - 1] * base;
            }
        }
    }

    return evaluateDFS(root.get(), 0, (domain == CoefDomain::Zk) ? (1 % modulus) : 1, powers);
}

int totalDegree(const std::vector<int>& exponents) {
    int degree = 0;
    for (int exp : exponents) {
        degree += exp;
    }
    return degree;
}

int PolynomialTrie::homeDegree() const {
    const std::vector<Term> ts = terms();
    if (ts.empty()) {
        return 0;
    }

    const int degree = totalDegree(ts.front().exponents);

    for (const auto& term : ts) {
        if (totalDegree(term.exponents) != degree) {
            return -1;
        }
    }

    return degree;
}

std::pair<PolynomialTrie, PolynomialTrie> PolynomialTrie::splitByDegree(int degree) const {
    PolynomialTrie homogeneousPart(vars, domain, modulus);
    PolynomialTrie remainder(vars, domain, modulus);

    for (const auto& term : terms()) {
        if (totalDegree(term.exponents) == degree) {
            homogeneousPart.addMonomial(term.coef, term.exponents);
        } else {
            remainder.addMonomial(term.coef, term.exponents);
        }
    }

    return {homogeneousPart, remainder};
}

bool PolynomialTrie::isGreaterLex(
    const std::vector<int>& a,
    const std::vector<int>& b
) {
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (a[i] > b[i]) {
            return true;
        }
        if (a[i] < b[i]) {
            return false;
        }
    }
    return false;
}

bool PolynomialTrie::isGreaterInvLex(
    const std::vector<int>& a,
    const std::vector<int>& b
) {
    for (int i = static_cast<int>(a.size()) - 1; i >= 0; --i) {
        if (a[i] > b[i]) {
            return true;
        }
        if (a[i] < b[i]) {
            return false;
        }
    }
    return false;
}

bool PolynomialTrie::isGreaterRevLex(
    const std::vector<int>& a,
    const std::vector<int>& b
) {
    for (int i = static_cast<int>(a.size()) - 1; i >= 0; --i) {
        if (a[i] < b[i]) {
            return true;
        }
        if (a[i] > b[i]) {
            return false;
        }
    }
    return false;
}

int PolynomialTrie::compareTotalDegree(
    const std::vector<int>& a,
    const std::vector<int>& b
) {
    int da = totalDegree(a);
    int db = totalDegree(b);

    if (da > db) {
        return 1;
    }
    if (da < db) {
        return -1;
    }
    return 0;
}

bool PolynomialTrie::isGreaterMonomial(
    const std::vector<int>& a,
    const std::vector<int>& b,
    MonomialOrder order
) {
    if (a.size() != b.size()) {
        throw std::runtime_error("Monomials have different dimension.");
    }

    switch (order) {
        case MonomialOrder::Lex:
            return isGreaterLex(a, b);

        case MonomialOrder::GrLex: {
            int degreeCmp = compareTotalDegree(a, b);
            if (degreeCmp != 0) {
                return degreeCmp > 0;
            }
            return isGreaterLex(a, b);
        }

        case MonomialOrder::GrevLex: {
            int degreeCmp = compareTotalDegree(a, b);
            if (degreeCmp != 0) {
                return degreeCmp > 0;
            }
            return isGreaterRevLex(a, b);
        }

        case MonomialOrder::InvLex:
            return isGreaterInvLex(a, b);

        case MonomialOrder::RevLex:
            return isGreaterRevLex(a, b);
    }

    return false;
}

LeadingData PolynomialTrie::leadingData(MonomialOrder order) const {
    const std::vector<Term> ts = terms();

    LeadingData result;
    if (ts.empty()) {
        return result;
    }

    result.coef = ts[0].coef;
    result.exponents = ts[0].exponents;
    result.exists = true;

    for (std::size_t i = 1; i < ts.size(); ++i) {
        if (isGreaterMonomial(ts[i].exponents, result.exponents, order)) {
            result.coef = ts[i].coef;
            result.exponents = ts[i].exponents;
        }
    }

    result.coef = normalizeCoef(result.coef);
    return result;
}

std::vector<int> PolynomialTrie::leadingMonomial(MonomialOrder order) const {
    LeadingData d = leadingData(order);
    if (!d.exists) {
        throw std::runtime_error("Zero polynomial has no leading monomial.");
    }
    return d.exponents;
}

Coef PolynomialTrie::leadingCoefficient(MonomialOrder order) const {
    LeadingData d = leadingData(order);
    if (!d.exists) {
        throw std::runtime_error("Zero polynomial has no leading coefficient.");
    }
    return d.coef;
}

Term PolynomialTrie::leadingTerm(MonomialOrder order) const {
    LeadingData d = leadingData(order);
    if (!d.exists) {
        throw std::runtime_error("Zero polynomial has no leading term.");
    }
    return Term{d.coef, d.exponents};
}

std::vector<int> PolynomialTrie::multiDegree(MonomialOrder order) const {
    return leadingMonomial(order);
}

std::string PolynomialTrie::monomialOnlyToString(const std::vector<int>& exponents) const {
    checkExponents(exponents);

    bool hasVariables = false;
    for (int exp : exponents) {
        if (exp != 0) {
            hasVariables = true;
            break;
        }
    }

    if (!hasVariables) {
        return "1";
    }

    std::ostringstream out;
    for (int i = 0; i < varCount(); ++i) {
        int exp = exponents[i];
        if (exp == 0) {
            continue;
        }

        out << vars[i];
        if (exp != 1) {
            out << "^" << exp;
        }
    }

    return out.str();
}

std::string PolynomialTrie::monomialToString(const Term& t) const {
    Coef coef = displayCoef(t.coef);

    bool hasVariablePart = false;
    for (int exp : t.exponents) {
        if (exp != 0) {
            hasVariablePart = true;
            break;
        }
    }

    if (!hasVariablePart) {
        return std::to_string(coef);
    }

    std::ostringstream out;

    if (coef == -1) {
        out << "-";
    } else if (coef != 1) {
        out << coef;
    }

    for (int i = 0; i < varCount(); ++i) {
        int exp = t.exponents[i];
        if (exp == 0) {
            continue;
        }

        out << vars[i];
        if (exp != 1) {
            out << "^" << exp;
        }
    }

    return out.str();
}

std::string PolynomialTrie::toString() const {
    std::vector<Term> ts = terms();
    if (ts.empty()) {
        return "0";
    }

    std::sort(ts.begin(), ts.end(), [](const Term& a, const Term& b) {
        int da = totalDegree(a.exponents);
        int db = totalDegree(b.exponents);

        if (da != db) {
            return da > db;
        }
        return a.exponents > b.exponents;
    });

    std::ostringstream out;
    bool first = true;

    for (const auto& term : ts) {
        if (normalizeCoef(term.coef) == 0) {
            continue;
        }

        Coef shown = displayCoef(term.coef);
        std::string piece = monomialToString(term);

        if (first) {
            out << piece;
            first = false;
        } else if (shown < 0) {
            if (!piece.empty() && piece[0] == '-') {
                out << " - " << piece.substr(1);
            } else {
                out << " - " << piece;
            }
        } else {
            out << " + " << piece;
        }
    }

    return first ? "0" : out.str();
}

std::string PolynomialTrie::toString(MonomialOrder order) const {
    std::vector<Term> ts = terms();
    if (ts.empty()) {
        return "0";
    }

    std::sort(ts.begin(), ts.end(), [order](const Term& a, const Term& b) {
        return PolynomialTrie::isGreaterMonomial(a.exponents, b.exponents, order);
    });

    std::ostringstream out;
    bool first = true;

    for (const auto& term : ts) {
        if (normalizeCoef(term.coef) == 0) {
            continue;
        }

        Coef shown = displayCoef(term.coef);
        std::string piece = monomialToString(term);

        if (first) {
            out << piece;
            first = false;
        } else if (shown < 0) {
            if (!piece.empty() && piece[0] == '-') {
                out << " - " << piece.substr(1);
            } else {
                out << " - " << piece;
            }
        } else {
            out << " + " << piece;
        }
    }

    return first ? "0" : out.str();
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

Coef fastPowMod(Coef a, int e, Coef mod) {
    if (mod <= 0) {
        throw std::runtime_error("Modulus must be positive.");
    }
    if (e < 0) {
        throw std::runtime_error("Exponent must be non-negative.");
    }

    a %= mod;
    if (a < 0) {
        a += mod;
    }

    Coef result = 1 % mod;

    while (e > 0) {
        if (e & 1) {
            result = mulMod(result, a, mod);
        }
        a = mulMod(a, a, mod);
        e >>= 1;
    }

    return result;
}