#include "Polynomial.h"

#include <sstream>

PolynomialTrie::PolynomialTrie() : root(std::make_unique<Node>()), modulus(2) {}

PolynomialTrie::PolynomialTrie(const vector<string> &varNames, int mod)
    : root(std::make_unique<Node>()), vars(varNames), modulus(mod) {
    if (modulus <= 0) {
        throw std::runtime_error("Модуль k должен быть положительным.");
    }
}

std::unique_ptr<PolynomialTrie::Node> PolynomialTrie::cloneNode(const Node* node) const {
    if (!node) {
        return nullptr;
    }

    auto newNode = std::make_unique<Node>();
    newNode->coef = node->coef;
    newNode->hasCoef = node->hasCoef;

    for (const auto& [exp, child] : node->children) {
        newNode->children[exp] = cloneNode(child.get());
    }

    return newNode;
}

PolynomialTrie::PolynomialTrie(const PolynomialTrie& other) : root(cloneNode(other.root.get())),
      vars(other.vars), modulus(other.modulus) {}

PolynomialTrie& PolynomialTrie::operator=(const PolynomialTrie& other) {
    if (this == &other) {
        return *this;
    }

    vars = other.vars;
    modulus = other.modulus;
    root = cloneNode(other.root.get());

    return *this;
}

int PolynomialTrie::varCount() const {
    return static_cast<int>(vars.size());
}

void PolynomialTrie::collectTermsDFS(const Node *node, int level, vector<int> &path, vector<Term> &result) const {
    if (!node) {
        return;
    }

    if (level == varCount()) {
        if (node->hasCoef && node->coef != 0) {
            result.push_back({node->coef, path});
        }
        return;
    }

    for (const auto& [exp, child] : node->children) {
        path[level] = exp;
        collectTermsDFS(child.get(), level + 1, path, result);
    }
}

void PolynomialTrie::checkCompatible(const PolynomialTrie &other) const {
    if (vars != other.vars) {
        throw std::runtime_error("Многочлены заданы над разными наборами переменных.");
    }

    if (modulus != other.modulus) {
        throw std::runtime_error("Многочлены заданы над разными колцами Z_k.");
    }
}

const vector<string>& PolynomialTrie::variableNames() const {
    return vars;
}

int PolynomialTrie::getModulus() const {
    return modulus;
}

void PolynomialTrie::setVariableNames(const vector<string> &varNames) {
    vars = varNames;
    clear();
}

void PolynomialTrie::setModulus(int mod) {
    if (mod <= 0) {
        throw std::runtime_error("Модуль k должен быть положительным.");
    }
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

void PolynomialTrie::addMonomial(int coef, const vector<int> &exponents) {
    if (static_cast<int>(exponents.size()) != varCount()) {
        throw std::runtime_error("Некорректное число степеней.");
    }

    coef = normalizeCoef(coef);
    if (coef == 0) {
        return;
    }

    Node* cur = root.get();

    for (int i = 0; i < varCount(); i++) {
        int exp = exponents[i];

        if (exp < 0) {
            throw std::runtime_error("Степени не могут быть отрицательными.");
        }

        if  (!cur->children.count(exp)) {
            cur->children[exp] = std::make_unique<Node>();
        }

        cur = cur->children[exp].get();
    }

    if (cur->hasCoef) {
        cur->coef = normalizeCoef(cur->coef + coef);
    } else {
        cur->coef = coef;
        cur->hasCoef = true;
    }

    if (cur->hasCoef && cur->coef == 0) {
        cur->coef = 0;
        cur->hasCoef = false;
    }
}

void PolynomialTrie::setMonomial(int coef, const vector<int> &exponents) {
    if (static_cast<int>(exponents.size()) != varCount()) {
        throw std::runtime_error("Некорректное число степеней.");
    }

    Node* cur = root.get();

    for (int i = 0; i < varCount(); i++) {
        int exp = exponents[i];
        if (exp < 0) {
            throw std::runtime_error("Степени не могут быть отрицательными.");
        }

        if (!cur->children.count(exp)) {
            cur->children[exp] = std::make_unique<Node>();
        }

        cur = cur->children[exp].get();
    }

    coef = normalizeCoef(coef);


    if (coef == 0) {
        cur->coef = 0;
        cur->hasCoef = false;
    } else {
        cur->coef = coef;
        cur->hasCoef = true;
    }
}

vector<Term> PolynomialTrie::terms() const {
    vector<Term> result;
    std::vector<int> path(varCount(), 0);
    collectTermsDFS(root.get(), 0, path, result);
    return result;
}

void PolynomialTrie::normalize() {
    vector<Term> ts = terms();
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
    for (const auto& t : other.terms()) {
        addMonomial(t.coef, t.exponents);
    }

    return *this;
}

PolynomialTrie& PolynomialTrie::operator-=(const PolynomialTrie& other) {
    checkCompatible(other);

    for (const auto& t : other.terms()) {
        addMonomial(-t.coef, t.exponents);
    }
    return *this;
}

PolynomialTrie& PolynomialTrie::operator*=(const PolynomialTrie& other) {
    checkCompatible(other);

    const std::vector<Term> left = terms();
    const std::vector<Term> right = other.terms();

    clear();

    for (const auto& a : left) {
        for (const auto& b : right) {
            std::vector<int> exps(varCount());
            for (int i = 0; i < varCount(); ++i) {
                exps[i] = a.exponents[i] + b.exponents[i];
            }
            addMonomial(a.coef * b.coef, exps);
        }
    }

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

    for (size_t i = 0; i < a.size(); ++i) {
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

vector<vector<int> > PolynomialTrie::support() const {
    vector<vector<int> > result;
    for (const auto& term : terms()) {
        result.push_back(term.exponents);
    }
    return result;
}

int fastPowMod(int a, int e, int mod) {
    a %= mod;
    if (a < 0) {
        a += mod;
    }

    int result = 1 % mod;

    while (e > 0) {
        if (e & 1) {
            result = (result * a) % mod;
        }
        a = (a * a) % mod;
        e >>= 1;
    }

    return result;
}

int PolynomialTrie::evaluateAt(const vector<int> &point) const {
    if (static_cast<int>(point.size()) != varCount()) {
        throw std::runtime_error("Размер точки не совпадает с числом переменных.");
    }

    int result = 0;

    for (const auto& t : terms()) {
        int value = normalizeCoef(t.coef);

        for (int i = 0; i < varCount(); ++i) {
            const int base = normalizeCoef(point[i]);
            const int pw = fastPowMod(base, t.exponents[i], modulus);
            value = normalizeCoef(value * pw);
        }

        result = normalizeCoef(result + value);
    }

    return result;
}

int totalDegree(const std::vector<int>& exponents) {
    int sum = 0;
    for (int x : exponents) {
        sum += x;
    }
    return sum;
}

int PolynomialTrie::homeDegree() const {
    const std::vector<Term> ts = terms();

    if (ts.empty()) {
        return 0;
    }

    const int d = totalDegree(ts[0].exponents);

    for (const auto& t : ts) {
        if (totalDegree(t.exponents) != d) {
            return -1;
        }
    }

    return d;
}

std::pair<PolynomialTrie, PolynomialTrie> PolynomialTrie::splitByDegree(int degree) const {
    PolynomialTrie h(vars, modulus);
    PolynomialTrie g(vars, modulus);

    for (const auto& t : terms()) {
        if (totalDegree(t.exponents) == degree) {
            h.addMonomial(t.coef, t.exponents);
        } else {
            g.addMonomial(t.coef, t.exponents);
        }
    }

    return {h, g};
}

std::string PolynomialTrie::monomialToString(const Term& t) const {
    if (t.coef == 0) {
        return "0";
    }

    bool hasVariables = false;
    for (int e : t.exponents) {
        if (e != 0) {
            hasVariables = true;
            break;
        }
    }

    std::ostringstream out;
    const int c = normalizeCoef(t.coef);

    if (!hasVariables) {
        out << c;
        return out.str();
    }

    if (c != 1) {
        out << c;
    }

    for (int i = 0; i < varCount(); ++i) {
        const int e = t.exponents[i];
        if (e == 0) {
            continue;
        }

        out << vars[i];
        if (e != 1) {
            out << "^" << e;
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
        const int da = totalDegree(a.exponents);
        const int db = totalDegree(b.exponents);

        if (da != db) {
            return da > db;
        }
        return a.exponents > b.exponents;
    });

    std::ostringstream out;
    bool first = true;

    for (const auto& t : ts) {
        if (normalizeCoef(t.coef) == 0) {
            continue;
        }

        if (!first) {
            out << " + ";
        }

        out << monomialToString(t);
        first = false;
    }

    const std::string result = out.str();
    return result.empty() ? "0" : result;
}
