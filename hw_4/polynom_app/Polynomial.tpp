#pragma once

#include <algorithm>
#include <sstream>
#include <stdexcept>

using std::max;
using std::move;
using std::ostringstream;
using std::runtime_error;
using std::size_t;
using std::sort;
using std::string;
using std::vector;

template <class Ops>
Polynomial<Ops>::Polynomial(const vector<string>& varNames)
    : vars(varNames) {}

template <class Ops>
int Polynomial<Ops>::varCount() const {
    return static_cast<int>(vars.size());
}

template <class Ops>
const vector<string>& Polynomial<Ops>::variableNames() const {
    return vars;
}

template <class Ops>
void Polynomial<Ops>::setVariableNames(const vector<string>& varNames) {
    vars = varNames;
    clear();
}

template <class Ops>
void Polynomial<Ops>::clear() {
    data.clear();
}

template <class Ops>
void Polynomial<Ops>::checkExponents(const vector<int>& exponents) const {
    if (static_cast<int>(exponents.size()) != varCount()) {
        throw runtime_error("Wrong number of exponents.");
    }

    for (int e : exponents) {
        if (e < 0) {
            throw runtime_error("Negative exponents are not allowed.");
        }
    }
}

template <class Ops>
void Polynomial<Ops>::checkSameVariables(const Polynomial& other) const {
    if (vars != other.vars) {
        throw runtime_error("Polynomials use different variables.");
    }
}

template <class Ops>
void Polynomial<Ops>::checkSameVariables(const vector<Polynomial>& basis) const {
    for (const auto& g : basis) {
        if (g.variableNames() != vars) {
            throw runtime_error("Basis polynomials use different variables.");
        }
    }
}

template <class Ops>
void Polynomial<Ops>::checkSameMonomialDimension(
    const vector<int>& a,
    const vector<int>& b
) {
    if (a.size() != b.size()) {
        throw runtime_error("Monomials have different dimension.");
    }
}

template <class Ops>
bool Polynomial<Ops>::sameMonomial(const vector<int>& a, const vector<int>& b) {
    return a == b;
}

template <class Ops>
void Polynomial<Ops>::combineLikeTerms() {
    vector<Term> result;
    result.reserve(data.size());

    for (const auto& t : data) {
        if (Ops::isZero(t.coef)) {
            continue;
        }

        bool found = false;
        for (auto& r : result) {
            if (sameMonomial(r.exponents, t.exponents)) {
                r.coef = Ops::add(r.coef, t.coef);
                found = true;
                break;
            }
        }

        if (!found) {
            result.push_back(t);
        }
    }

    data.clear();
    data.reserve(result.size());

    for (const auto& t : result) {
        if (!Ops::isZero(t.coef)) {
            data.push_back(t);
        }
    }
}

template <class Ops>
void Polynomial<Ops>::addMonomial(const Coef& coef, const vector<int>& exponents) {
    checkExponents(exponents);

    if (Ops::isZero(coef)) {
        return;
    }

    for (auto& t : data) {
        if (sameMonomial(t.exponents, exponents)) {
            t.coef = Ops::add(t.coef, coef);
            combineLikeTerms();
            return;
        }
    }

    data.push_back({coef, exponents});
}

template <class Ops>
void Polynomial<Ops>::setMonomial(const Coef& coef, const vector<int>& exponents) {
    checkExponents(exponents);

    for (auto& t : data) {
        if (sameMonomial(t.exponents, exponents)) {
            t.coef = coef;
            combineLikeTerms();
            return;
        }
    }

    if (!Ops::isZero(coef)) {
        data.push_back({coef, exponents});
    }
}

template <class Ops>
const vector<typename Polynomial<Ops>::Term>& Polynomial<Ops>::terms() const {
    return data;
}

template <class Ops>
bool Polynomial<Ops>::isZero() const {
    return data.empty();
}

template <class Ops>
Polynomial<Ops>& Polynomial<Ops>::operator+=(const Polynomial& other) {
    checkSameVariables(other);

    for (const auto& t : other.data) {
        addMonomial(t.coef, t.exponents);
    }

    return *this;
}

template <class Ops>
Polynomial<Ops>& Polynomial<Ops>::operator-=(const Polynomial& other) {
    checkSameVariables(other);

    for (const auto& t : other.data) {
        addMonomial(Ops::sub(Coef{}, t.coef), t.exponents);
    }

    return *this;
}

template <class Ops>
Polynomial<Ops>& Polynomial<Ops>::operator*=(const Polynomial& other) {
    checkSameVariables(other);

    Polynomial result(vars);

    for (const auto& a : data) {
        for (const auto& b : other.data) {
            vector<int> exponents(varCount(), 0);
            for (int i = 0; i < varCount(); ++i) {
                exponents[i] = a.exponents[i] + b.exponents[i];
            }

            result.addMonomial(Ops::mul(a.coef, b.coef), exponents);
        }
    }

    *this = move(result);
    return *this;
}

template <class Ops>
Polynomial<Ops> Polynomial<Ops>::operator+(const Polynomial& other) const {
    Polynomial result = *this;
    result += other;
    return result;
}

template <class Ops>
Polynomial<Ops> Polynomial<Ops>::operator-(const Polynomial& other) const {
    Polynomial result = *this;
    result -= other;
    return result;
}

template <class Ops>
Polynomial<Ops> Polynomial<Ops>::operator*(const Polynomial& other) const {
    Polynomial result = *this;
    result *= other;
    return result;
}

template <class Ops>
bool Polynomial<Ops>::operator==(const Polynomial& other) const {
    checkSameVariables(other);

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

    sort(a.data.begin(), a.data.end(), cmp);
    sort(b.data.begin(), b.data.end(), cmp);

    for (size_t i = 0; i < a.data.size(); ++i) {
        if (!Ops::equal(a.data[i].coef, b.data[i].coef)) {
            return false;
        }
        if (a.data[i].exponents != b.data[i].exponents) {
            return false;
        }
    }

    return true;
}

template <class Ops>
bool Polynomial<Ops>::operator!=(const Polynomial& other) const {
    return !(*this == other);
}

template <class Ops>
vector<vector<int>> Polynomial<Ops>::support() const {
    vector<vector<int>> result;
    result.reserve(data.size());

    for (const auto& t : data) {
        result.push_back(t.exponents);
    }

    return result;
}

template <class Ops>
typename Polynomial<Ops>::Coef Polynomial<Ops>::evaluateAt(const vector<Coef>& point) const {
    if (static_cast<int>(point.size()) != varCount()) {
        throw runtime_error("Point size does not match variable count.");
    }

    Coef result{};

    for (const auto& t : data) {
        Coef value = t.coef;

        for (int i = 0; i < varCount(); ++i) {
            for (int p = 0; p < t.exponents[i]; ++p) {
                value = Ops::mul(value, point[i]);
            }
        }

        result = Ops::add(result, value);
    }

    return result;
}

template <class Ops>
int Polynomial<Ops>::homeDegree() const {
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

template <class Ops>
pair<Polynomial<Ops>, Polynomial<Ops>> Polynomial<Ops>::splitByDegree(int degree) const {
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

template <class Ops>
bool Polynomial<Ops>::isGreaterLex(
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

template <class Ops>
bool Polynomial<Ops>::isGreaterGrevLexTie(
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

template <class Ops>
int Polynomial<Ops>::compareTotalDegree(
    const vector<int>& a,
    const vector<int>& b
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

template <class Ops>
vector<int> Polynomial<Ops>::eraseVariableAt(
    const vector<int>& exponents,
    int varIndex
) const {
    if (varIndex < 0 || varIndex >= static_cast<int>(exponents.size())) {
        throw runtime_error("Wrong variable index.");
    }

    vector<int> result;
    result.reserve(exponents.size() - 1);

    for (int i = 0; i < static_cast<int>(exponents.size()); ++i) {
        if (i != varIndex) {
            result.push_back(exponents[i]);
        }
    }

    return result;
}

template <class Ops>
bool Polynomial<Ops>::isGreaterMonomial(
    const vector<int>& a,
    const vector<int>& b,
    const MonomialView& view
) {
    checkSameMonomialDimension(a, b);

    vector<int> aa = a;
    vector<int> bb = b;

    if (view.ignoresZ()) {
        if (view.zIndex < 0 || view.zIndex >= static_cast<int>(a.size())) {
            throw runtime_error("Wrong z variable index.");
        }

        aa.erase(aa.begin() + view.zIndex);
        bb.erase(bb.begin() + view.zIndex);
    }

    switch (view.order) {
        case MonomialOrder::Lex:
            return isGreaterLex(aa, bb);

        case MonomialOrder::GrLex: {
            int cmp = compareTotalDegree(aa, bb);
            if (cmp != 0) {
                return cmp > 0;
            }
            return isGreaterLex(aa, bb);
        }

        case MonomialOrder::GrevLex: {
            int cmp = compareTotalDegree(aa, bb);
            if (cmp != 0) {
                return cmp > 0;
            }
            return isGreaterGrevLexTie(aa, bb);
        }
    }

    return false;
}

template <class Ops>
typename Polynomial<Ops>::LeadingData Polynomial<Ops>::leadingData(const MonomialView& view) const {
    LeadingData result;

    if (data.empty()) {
        return result;
    }

    result.coef = data[0].coef;
    result.exponents = data[0].exponents;
    result.exists = true;

    for (size_t i = 1; i < data.size(); ++i) {
        if (isGreaterMonomial(data[i].exponents, result.exponents, view)) {
            result.coef = data[i].coef;
            result.exponents = data[i].exponents;
        }
    }

    return result;
}

template <class Ops>
vector<int> Polynomial<Ops>::leadingMonomial(const MonomialView& view) const {
    LeadingData d = leadingData(view);

    if (!d.exists) {
        throw runtime_error("Zero polynomial has no leading monomial.");
    }

    return d.exponents;
}

template <class Ops>
typename Polynomial<Ops>::Coef Polynomial<Ops>::leadingCoefficient(const MonomialView& view) const {
    LeadingData d = leadingData(view);

    if (!d.exists) {
        throw runtime_error("Zero polynomial has no leading coefficient.");
    }

    return d.coef;
}

template <class Ops>
typename Polynomial<Ops>::Term Polynomial<Ops>::leadingTerm(const MonomialView& view) const {
    LeadingData d = leadingData(view);

    if (!d.exists) {
        throw runtime_error("Zero polynomial has no leading term.");
    }

    return {d.coef, d.exponents};
}

template <class Ops>
vector<int> Polynomial<Ops>::multiDegree(const MonomialView& view) const {
    vector<int> lm = leadingMonomial(view);

    if (view.ignoresZ()) {
        return eraseVariableAt(lm, view.zIndex);
    }

    return lm;
}

template <class Ops>
vector<int> Polynomial<Ops>::lcmMonomial(
    const vector<int>& a,
    const vector<int>& b
) const {
    checkSameMonomialDimension(a, b);

    vector<int> result(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        result[i] = max(a[i], b[i]);
    }
    return result;
}

template <class Ops>
vector<int> Polynomial<Ops>::subtractMonomial(
    const vector<int>& a,
    const vector<int>& b
) const {
    checkSameMonomialDimension(a, b);

    vector<int> result(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] < b[i]) {
            throw runtime_error("Negative exponent in monomial subtraction.");
        }
        result[i] = a[i] - b[i];
    }
    return result;
}

template <class Ops>
bool Polynomial<Ops>::dividesMonomial(
    const vector<int>& a,
    const vector<int>& b
) const {
    checkSameMonomialDimension(a, b);

    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] > b[i]) {
            return false;
        }
    }
    return true;
}

template <class Ops>
typename Polynomial<Ops>::Coef Polynomial<Ops>::exactDivide(const Coef& a, const Coef& b) const {
    return Ops::divide(a, b);
}

template <class Ops>
vector<int> Polynomial<Ops>::multiDegreePair(
    const Polynomial& other,
    const MonomialView& view
) const {
    checkSameVariables(other);

    if (isZero() || other.isZero()) {
        throw runtime_error("multideg(f, g) is undefined for zero polynomial.");
    }

    vector<int> lmThis = leadingMonomial(view);
    vector<int> lmOther = other.leadingMonomial(view);

    if (view.ignoresZ()) {
        lmThis = eraseVariableAt(lmThis, view.zIndex);
        lmOther = eraseVariableAt(lmOther, view.zIndex);
    }

    checkSameMonomialDimension(lmThis, lmOther);

    vector<int> result(lmThis.size());
    for (size_t i = 0; i < lmThis.size(); ++i) {
        result[i] = max(lmThis[i], lmOther[i]);
    }

    return result;
}

template <class Ops>
Polynomial<Ops> Polynomial<Ops>::multiplyByMonomial(
    const Coef& coef,
    const vector<int>& exponents
) const {
    checkExponents(exponents);

    if (Ops::isZero(coef) || isZero()) {
        return Polynomial(vars);
    }

    Polynomial result(vars);
    result.data.reserve(data.size());

    for (const auto& t : data) {
        vector<int> newExp(varCount());
        for (int i = 0; i < varCount(); ++i) {
            newExp[i] = t.exponents[i] + exponents[i];
        }
        result.data.push_back({Ops::mul(t.coef, coef), std::move(newExp)});
    }

    return result;
}

template <class Ops>
Polynomial<Ops> Polynomial<Ops>::sPolynomial(
    const Polynomial& other,
    const MonomialView& view
) const {
    checkSameVariables(other);

    if (isZero() || other.isZero()) {
        throw runtime_error("S-polynomial is undefined for zero polynomial.");
    }

    const Term ltF = leadingTerm(view);
    const Term ltG = other.leadingTerm(view);

    const vector<int> lcmExp = lcmMonomial(ltF.exponents, ltG.exponents);
    const vector<int> mulFExp = subtractMonomial(lcmExp, ltF.exponents);
    const vector<int> mulGExp = subtractMonomial(lcmExp, ltG.exponents);

    Polynomial left = multiplyByMonomial(ltG.coef, mulFExp);
    Polynomial right = other.multiplyByMonomial(ltF.coef, mulGExp);

    return left - right;
}

template <class Ops>
Polynomial<Ops> Polynomial<Ops>::remainderOnDivision(
    const vector<Polynomial>& basis,
    const MonomialView& view
) const {
    checkSameVariables(basis);

    Polynomial p = *this;
    Polynomial r(vars);

    while (!p.isZero()) {
        const Term ltP = p.leadingTerm(view);
        bool reduced = false;

        for (const auto& g : basis) {
            if (g.isZero()) {
                continue;
            }

            const Term ltG = g.leadingTerm(view);

            if (!dividesMonomial(ltG.exponents, ltP.exponents)) {
                continue;
            }

            Coef coefQuot;
            try {
                coefQuot = exactDivide(ltP.coef, ltG.coef);
            } catch (...) {
                continue;
            }

            const vector<int> expQuot =
                subtractMonomial(ltP.exponents, ltG.exponents);

            p -= g.multiplyByMonomial(coefQuot, expQuot);
            reduced = true;
            break;
        }

        if (!reduced) {
            r.addMonomial(ltP.coef, ltP.exponents);
            p.addMonomial(Ops::sub(Coef{}, ltP.coef), ltP.exponents);
        }
    }

    return r;
}

template <class Ops>
bool Polynomial<Ops>::isGroebnerBasis(
    const vector<Polynomial>& basis,
    const MonomialView& view
) {
    if (basis.empty()) {
        return true;
    }

    const vector<string>& refVars = basis.front().variableNames();

    for (const auto& g : basis) {
        if (g.variableNames() != refVars) {
            throw runtime_error("Basis polynomials use different variables.");
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

            const Polynomial s = basis[i].sPolynomial(basis[j], view);
            const Polynomial rem = s.remainderOnDivision(basis, view);

            if (!rem.isZero()) {
                return false;
            }
        }
    }

    return true;
}

template <class Ops>
vector<Polynomial<Ops>> Polynomial<Ops>::buchbergerBasis(
    const vector<Polynomial>& generators,
    const MonomialView& view
) {
    if (generators.empty()) {
        return {};
    }

    const vector<string>& refVars = generators.front().variableNames();

    for (const auto& f : generators) {
        if (f.variableNames() != refVars) {
            throw runtime_error("Generators use different variables.");
        }
    }

    vector<Polynomial> G;
    G.reserve(generators.size());

    for (const auto& f : generators) {
        if (!f.isZero()) {
            G.push_back(f);
        }
    }

    bool changed = true;

    while (changed) {
        changed = false;
        vector<Polynomial> Gprev = G;

        for (size_t i = 0; i < Gprev.size(); ++i) {
            for (size_t j = i + 1; j < Gprev.size(); ++j) {
                Polynomial s = Gprev[i].sPolynomial(Gprev[j], view);
                Polynomial r = s.remainderOnDivision(Gprev, view);

                if (!r.isZero()) {
                    G.push_back(r);
                    changed = true;
                }
            }
        }
    }

    return G;
}

template <class Ops>
string Polynomial<Ops>::monomialOnlyToString(const vector<int>& exponents) const {
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

    ostringstream out;
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

template <class Ops>
string Polynomial<Ops>::monomialToString(const Term& t) const {
    bool hasVars = false;
    for (int e : t.exponents) {
        if (e != 0) {
            hasVars = true;
            break;
        }
    }

    const string coefStr = Ops::toString(t.coef);

    if (!hasVars) {
        return coefStr;
    }

    ostringstream out;

    if (coefStr != "1") {
        out << coefStr;
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

template <class Ops>
string Polynomial<Ops>::toString() const {
    return toString(MonomialView{MonomialOrder::Lex, -1});
}

template <class Ops>
string Polynomial<Ops>::toString(const MonomialView& view) const {
    if (data.empty()) {
        return "0";
    }

    vector<Term> ts = data;
    sort(ts.begin(), ts.end(), [&view](const Term& a, const Term& b) {
        return Polynomial::isGreaterMonomial(a.exponents, b.exponents, view);
    });

    ostringstream out;
    bool first = true;

    for (const auto& t : ts) {
        if (Ops::isZero(t.coef)) {
            continue;
        }

        const string piece = monomialToString(t);

        if (first) {
            out << piece;
            first = false;
        } else {
            out << " + " << piece;
        }
    }

    return first ? "0" : out.str();
}
