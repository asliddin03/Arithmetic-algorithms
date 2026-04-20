#pragma once

#include "CoeffTypes.h"

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using std::pair;
using std::runtime_error;
using std::string;
using std::vector;

enum class MonomialOrder {
    Lex,
    GrLex,
    GrevLex
};

struct MonomialView {
    MonomialOrder order = MonomialOrder::Lex;
    int zIndex = -1;

    bool ignoresZ() const {
        return zIndex >= 0;
    }
};

template <class Coef>
struct TermT {
    Coef coef{};
    vector<int> exponents;
};

template <class Coef>
struct LeadingDataT {
    Coef coef{};
    vector<int> exponents;
    bool exists = false;
};

template <class Ops>
class Polynomial {
public:
    using OpsType = Ops;
    using Coef = typename Ops::Coef;
    using Term = TermT<Coef>;
    using LeadingData = LeadingDataT<Coef>;

private:
    vector<string> vars;
    vector<Term> data;

private:
    void checkExponents(const vector<int>& exponents) const;

    void checkSameVariables(const Polynomial& other) const;
    void checkSameVariables(const vector<Polynomial>& basis) const;

    static void checkSameMonomialDimension(
        const vector<int>& a,
        const vector<int>& b
    );

    void combineLikeTerms();

    static bool sameMonomial(const vector<int>& a, const vector<int>& b);
    static bool isGreaterLex(const vector<int>& a, const vector<int>& b);
    static bool isGreaterGrevLexTie(const vector<int>& a, const vector<int>& b);
    static int compareTotalDegree(const vector<int>& a, const vector<int>& b);

    vector<int> lcmMonomial(
        const vector<int>& a,
        const vector<int>& b
    ) const;

    vector<int> subtractMonomial(
        const vector<int>& a,
        const vector<int>& b
    ) const;

    bool dividesMonomial(
        const vector<int>& a,
        const vector<int>& b
    ) const;

    Coef exactDivide(const Coef& a, const Coef& b) const;

    vector<int> eraseVariableAt(
        const vector<int>& exponents,
        int varIndex
    ) const;

public:
    Polynomial() = default;
    explicit Polynomial(const vector<string>& varNames);

    int varCount() const;
    const vector<string>& variableNames() const;

    void setVariableNames(const vector<string>& varNames);
    void clear();

    void addMonomial(const Coef& coef, const vector<int>& exponents);
    void setMonomial(const Coef& coef, const vector<int>& exponents);

    const vector<Term>& terms() const;
    bool isZero() const;

    Polynomial& operator+=(const Polynomial& other);
    Polynomial& operator-=(const Polynomial& other);
    Polynomial& operator*=(const Polynomial& other);

    Polynomial operator+(const Polynomial& other) const;
    Polynomial operator-(const Polynomial& other) const;
    Polynomial operator*(const Polynomial& other) const;

    bool operator==(const Polynomial& other) const;
    bool operator!=(const Polynomial& other) const;

    vector<vector<int>> support() const;
    Coef evaluateAt(const vector<Coef>& point) const;
    int homeDegree() const;
    pair<Polynomial, Polynomial> splitByDegree(int degree) const;

    LeadingData leadingData(const MonomialView& view) const;
    vector<int> leadingMonomial(const MonomialView& view) const;
    Coef leadingCoefficient(const MonomialView& view) const;
    Term leadingTerm(const MonomialView& view) const;
    vector<int> multiDegree(const MonomialView& view) const;

    vector<int> multiDegreePair(
        const Polynomial& other,
        const MonomialView& view
    ) const;

    Polynomial multiplyByMonomial(
        const Coef& coef,
        const vector<int>& exponents
    ) const;

    Polynomial sPolynomial(
        const Polynomial& other,
        const MonomialView& view
    ) const;

    Polynomial remainderOnDivision(
        const vector<Polynomial>& basis,
        const MonomialView& view
    ) const;

    static bool isGroebnerBasis(
        const vector<Polynomial>& basis,
        const MonomialView& view
    );

    static vector<Polynomial> buchbergerBasis(
        const vector<Polynomial>& generators,
        const MonomialView& view
    );

    string monomialOnlyToString(const vector<int>& exponents) const;
    string monomialToString(const Term& t) const;
    string toString() const;
    string toString(const MonomialView& view) const;

    static bool isGreaterMonomial(
        const vector<int>& a,
        const vector<int>& b,
        const MonomialView& view
    );
};

inline int totalDegree(const vector<int>& exponents) {
    int s = 0;
    for (int x : exponents) {
        s += x;
    }
    return s;
}

#include "Polynomial.tpp"