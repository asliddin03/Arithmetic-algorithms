#pragma once

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using Coef = long long;

struct Term {
    Coef coef = 0;
    std::vector<int> exponents;
};

enum class CoefDomain {
    Z,
    Zk
};

enum class MonomialOrder {
    Lex,
    GrLex,
    GrevLex,
    InvLex,
    RevLex
};

struct LeadingData {
    Coef coef = 0;
    std::vector<int> exponents;
    bool exists = false;
};

class PolynomialTrie {
private:
    struct Node {
        std::map<int, std::unique_ptr<Node>> children;
        Coef coef = 0;
        bool hasCoef = false;
    };

    std::unique_ptr<Node> root;
    std::vector<std::string> vars;
    CoefDomain domain = CoefDomain::Zk;
    Coef modulus = 2;

private:
    std::unique_ptr<Node> cloneNode(const Node* node) const;

    void checkModulus(Coef mod) const;
    void checkExponents(const std::vector<int>& exponents) const;
    void checkCompatible(const PolynomialTrie& other) const;

    void collectTermsDFS(
        const Node* node,
        int level,
        std::vector<int>& path,
        std::vector<Term>& result
    ) const;

    void collectMaxExponentsDFS(
        const Node* node,
        int level,
        std::vector<int>& maxExp
    ) const;

    Coef evaluateDFS(
        const Node* node,
        int level,
        Coef currentValue,
        const std::vector<std::vector<Coef>>& powers
    ) const;

    static bool isGreaterLex(const std::vector<int>& a, const std::vector<int>& b);
    static bool isGreaterInvLex(const std::vector<int>& a, const std::vector<int>& b);
    static bool isGreaterRevLex(const std::vector<int>& a, const std::vector<int>& b);
    static int compareTotalDegree(const std::vector<int>& a, const std::vector<int>& b);

public:
    PolynomialTrie();
    PolynomialTrie(const std::vector<std::string>& varNames, CoefDomain domain, Coef mod = 2);

    PolynomialTrie(const PolynomialTrie& other);
    PolynomialTrie& operator=(const PolynomialTrie& other);

    PolynomialTrie(PolynomialTrie&& other) noexcept = default;
    PolynomialTrie& operator=(PolynomialTrie&& other) noexcept = default;

    int varCount() const;
    const std::vector<std::string>& variableNames() const;
    CoefDomain getDomain() const;
    Coef getModulus() const;

    void setVariableNames(const std::vector<std::string>& varNames);
    void setDomain(CoefDomain newDomain, Coef mod = 2);
    void setModulus(Coef mod);
    void clear();

    Coef normalizeCoef(Coef value) const;
    Coef displayCoef(Coef value) const;

    void addMonomial(Coef coef, const std::vector<int>& exponents);
    void setMonomial(Coef coef, const std::vector<int>& exponents);

    std::vector<Term> terms() const;
    void normalize();
    bool isZero() const;

    PolynomialTrie& operator+=(const PolynomialTrie& other);
    PolynomialTrie& operator-=(const PolynomialTrie& other);
    PolynomialTrie& operator*=(const PolynomialTrie& other);

    PolynomialTrie operator+(const PolynomialTrie& other) const;
    PolynomialTrie operator-(const PolynomialTrie& other) const;
    PolynomialTrie operator*(const PolynomialTrie& other) const;

    bool operator==(const PolynomialTrie& other) const;
    bool operator!=(const PolynomialTrie& other) const;

    std::vector<std::vector<int>> support() const;
    Coef evaluateAt(const std::vector<Coef>& point) const;
    int homeDegree() const;
    std::pair<PolynomialTrie, PolynomialTrie> splitByDegree(int degree) const;

    LeadingData leadingData(MonomialOrder order) const;
    std::vector<int> leadingMonomial(MonomialOrder order) const;
    Coef leadingCoefficient(MonomialOrder order) const;
    Term leadingTerm(MonomialOrder order) const;
    std::vector<int> multiDegree(MonomialOrder order) const;

    std::string monomialToString(const Term& t) const;
    std::string monomialOnlyToString(const std::vector<int>& exponents) const;
    std::string toString() const;
    std::string toString(MonomialOrder order) const;

    static bool isGreaterMonomial(
        const std::vector<int>& a,
        const std::vector<int>& b,
        MonomialOrder order
    );
};

Coef fastPow(Coef a, int e);
Coef fastPowMod(Coef a, int e, Coef mod);
int totalDegree(const std::vector<int>& exponents);