#pragma once

#include <string>
#include <utility>
#include <vector>

using Coef = long long;
using std::vector;
using std::string;

struct Term {
    Coef coef = 0;
    vector<int> exponents;
};

enum class MonomialOrder {
    Lex,
    GrLex,
    GrevLex
};

struct LeadingData {
    Coef coef = 0;
    vector<int> exponents;
    bool exists = false;
};

class Polynomial {
private:
    vector<string> vars;
    vector<Term> data;

    void checkExponents(const vector<int>& exponents) const;
    void combineLikeTerms();

    static bool sameMonomial(const vector<int>& a, const vector<int>& b);
    static bool isGreaterLex(const vector<int>& a, const vector<int>& b);
    static bool isGreaterGrevLexTie(const vector<int>& a, const vector<int>& b);
    static int compareTotalDegree(const vector<int>& a, const vector<int>& b);
    
    vector<int> lcmMonomial(const vector<int>& a, const vector<int>& b) const;
    vector<int> subtractMonomial(const vector<int>& a, const vector<int>& b) const;
    bool dividesMonomial(const vector<int>& a, const vector<int>& b) const;
    Coef exactDivide(Coef a, Coef b) const;

public:
    Polynomial() = default;
    explicit Polynomial(const vector<string>& varNames);

    int varCount() const;
    const vector<string>& variableNames() const;

    void setVariableNames(const vector<string>& varNames);
    void clear();

    void addMonomial(Coef coef, const vector<int>& exponents);
    void setMonomial(Coef coef, const vector<int>& exponents);

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
    std::pair<Polynomial, Polynomial> splitByDegree(int degree) const;

    LeadingData leadingData(MonomialOrder order) const;
    vector<int> leadingMonomial(MonomialOrder order) const;
    Coef leadingCoefficient(MonomialOrder order) const;
    Term leadingTerm(MonomialOrder order) const;
    vector<int> multiDegree(MonomialOrder order) const;

    string monomialOnlyToString(const vector<int>& exponents) const;
    string monomialToString(const Term& t) const;
    string toString() const;
    string toString(MonomialOrder order) const;

    static bool isGreaterMonomial(const vector<int>& a, const vector<int>& b, MonomialOrder order );

    vector<int> multiDegreePair(const Polynomial& other, MonomialOrder order) const;
    Polynomial multiplyByMonomial(Coef coef, const vector<int>& exponents) const;
    Polynomial sPolynomial(const Polynomial& other, MonomialOrder order) const;
    Polynomial remainderOnDivision(const vector<Polynomial>& basis, MonomialOrder order) const;
    static bool isGroebnerBasis(const vector<Polynomial>& basis, MonomialOrder order);
};

Coef fastPow(Coef a, int e);
int totalDegree(const vector<int>& exponents);