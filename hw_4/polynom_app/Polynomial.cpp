#include "Polynomial.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace {

    int mulMod(int a, int b, int mod) {
        return static_cast<int>((static_cast<std::int64_t>(a) * b) % mod);
    }

}

PolynomialTrie::PolynomialTrie()
    : root(std::make_unique<Node>()), modulus(2) {}

PolynomialTrie::PolynomialTrie(const std::vector<std::string>& varNames, int mod)
    : root(std::make_unique<Node>()), vars(varNames), modulus(mod) {
    checkModulus(mod);
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
      modulus(other.modulus) {}

PolynomialTrie& PolynomialTrie::operator=(const PolynomialTrie& other) {
    if (this == &other) {
        return *this;
    }

    vars = other.vars;
    modulus = other.modulus;
    root = cloneNode(other.root.get());
    return *this;
}

void PolynomialTrie::checkModulus(int mod) const {
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
    if (modulus != other.modulus) {
        throw std::runtime_error("Polynomials use different modulus.");
    }
}

int PolynomialTrie::varCount() const {
    return static_cast<int>(vars.size());
}

const std::vector<std::string>& PolynomialTrie::variableNames() const {
    return vars;
}

int PolynomialTrie::getModulus() const {
    return modulus;
}

void PolynomialTrie::setVariableNames(const std::vector<std::string>& varNames) {
    vars = varNames;
    clear();
}

void PolynomialTrie::setModulus(int mod) {
    checkModulus(mod);
    modulus = mod;
    normalize();
}

void PolynomialTrie::clear() {
    root = std::make_unique<Node>();
}

int PolynomialTrie::normalizeCoef(int value) const {
    value %= modulus;
    if (value < 0) {
        value += modulus;
    }
    return value;
}

void PolynomialTrie::addMonomial(int coef, const std::vector<int>& exponents) {
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

void PolynomialTrie::setMonomial(int coef, const std::vector<int>& exponents) {
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

    PolynomialTrie result(vars, modulus);

    for (const auto& a : left) {
        for (const auto& b : right) {
            std::vector<int> exponents(varCount(), 0);
            for (int i = 0; i < varCount(); ++i) {
                exponents[i] = a.exponents[i] + b.exponents[i];
            }

            int coef = mulMod(normalizeCoef(a.coef), normalizeCoef(b.coef), modulus);
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

int PolynomialTrie::evaluateHornerRecursive(
    const std::vector<Term>& termsList,
    const std::vector<int>& point,
    int variableIndex
) const {
    if (termsList.empty()) {
        return 0;
    }

    if (variableIndex == varCount()) {
        int sum = 0;
        for (const auto& term : termsList) {
            sum += normalizeCoef(term.coef);
            sum %= modulus;
        }
        return sum;
    }

    std::map<int, std::vector<Term>, std::greater<int>> grouped;

    for (const auto& term : termsList) {
        int exp = term.exponents[variableIndex];

        Term reducedTerm;
        reducedTerm.coef = term.coef;
        reducedTerm.exponents = term.exponents;
        reducedTerm.exponents[variableIndex] = 0;

        grouped[exp].push_back(std::move(reducedTerm));
    }

    int x = normalizeCoef(point[variableIndex]);
    int result = 0;
    bool first = true;
    int previousDegree = 0;

    for (const auto& [degree, bucket] : grouped) {
        int coeffValue = evaluateHornerRecursive(bucket, point, variableIndex + 1);

        if (first) {
            result = coeffValue;
            previousDegree = degree;
            first = false;
            continue;
        }

        int gap = previousDegree - degree;
        result = mulMod(result, fastPowMod(x, gap, modulus), modulus);
        result = normalizeCoef(result + coeffValue);
        previousDegree = degree;
    }

    result = mulMod(result, fastPowMod(x, previousDegree, modulus), modulus);
    return result;
}

int PolynomialTrie::evaluateAt(const std::vector<int>& point) const {
    if (static_cast<int>(point.size()) != varCount()) {
        throw std::runtime_error("Point size does not match variable count.");
    }

    if (isZero() || modulus == 1) {
        return 0;
    }

    return evaluateHornerRecursive(terms(), point, 0);
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
    PolynomialTrie homogeneousPart(vars, modulus);
    PolynomialTrie remainder(vars, modulus);

    for (const auto& term : terms()) {
        if (totalDegree(term.exponents) == degree) {
            homogeneousPart.addMonomial(term.coef, term.exponents);
        } else {
            remainder.addMonomial(term.coef, term.exponents);
        }
    }

    return {homogeneousPart, remainder};
}

std::string PolynomialTrie::monomialToString(const Term& t) const {
    int coef = normalizeCoef(t.coef);

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

    if (coef != 1) {
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
        int coef = normalizeCoef(term.coef);
        if (coef == 0) {
            continue;
        }

        if (!first) {
            out << " + ";
        }

        out << monomialToString(term);
        first = false;
    }

    if (first) {
        return "0";
    }

    return out.str();
}

int fastPowMod(int a, int e, int mod) {
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

    int result = 1 % mod;

    while (e > 0) {
        if (e & 1) {
            result = mulMod(result, a, mod);
        }
        a = mulMod(a, a, mod);
        e >>= 1;
    }

    return result;
}