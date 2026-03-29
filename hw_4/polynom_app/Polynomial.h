#pragma once

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

struct Term {
    int coef = 0;
    std::vector<int> exponents;
};

class PolynomialTrie {
private:
    struct Node {
        std::map<int, std::unique_ptr<Node>> children;
        int coef = 0;
        bool hasCoef = false;
    };

    std::unique_ptr<Node> root;
    std::vector<std::string> vars;
    int modulus = 2;

    std::unique_ptr<Node> cloneNode(const Node* node) const;

    void checkModulus(int mod) const;
    void checkExponents(const std::vector<int>& exponents) const;
    void checkCompatible(const PolynomialTrie& other) const;

    void collectTermsDFS(
        const Node* node,
        int level,
        std::vector<int>& path,
        std::vector<Term>& result
    ) const;

    int evaluateHornerRecursive(
        const std::vector<Term>& termsList,
        const std::vector<int>& point,
        int variableIndex
    ) const;

public:
    PolynomialTrie();
    PolynomialTrie(const std::vector<std::string>& varNames, int mod);

    PolynomialTrie(const PolynomialTrie& other);
    PolynomialTrie& operator=(const PolynomialTrie& other);

    PolynomialTrie(PolynomialTrie&& other) noexcept = default;
    PolynomialTrie& operator=(PolynomialTrie&& other) noexcept = default;

    int varCount() const;
    const std::vector<std::string>& variableNames() const;
    int getModulus() const;

    void setVariableNames(const std::vector<std::string>& varNames);
    void setModulus(int mod);
    void clear();

    int normalizeCoef(int value) const;

    void addMonomial(int coef, const std::vector<int>& exponents);
    void setMonomial(int coef, const std::vector<int>& exponents);

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
    int evaluateAt(const std::vector<int>& point) const;
    int homeDegree() const;
    std::pair<PolynomialTrie, PolynomialTrie> splitByDegree(int degree) const;

    std::string monomialToString(const Term& t) const;
    std::string toString() const;
};

int fastPowMod(int a, int e, int mod);
int totalDegree(const std::vector<int>& exponents);