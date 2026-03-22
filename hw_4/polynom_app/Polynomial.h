#pragma once
#include <vector>
#include <map>
#include <string>

using std::map;
using std::vector;
using std::string;
using std::unique_ptr;

struct Term {
    int coef = 0;
    vector<int> exponents;
};

class PolynomialTrie {
private:
    struct Node {
        map<int, unique_ptr<Node>> children;
        int coef = 0;
        bool hasCoef = false;
    };

    unique_ptr<Node> root;
    vector<string> vars;
    int modulus = 2;

    void collectTermsDFS(const Node* node, int level, vector<int>& path,
        vector<Term>& result) const;

    void checkCompatible(const PolynomialTrie& other) const;
    std::unique_ptr<Node> cloneNode(const Node* node) const;

    public:
    PolynomialTrie();
    PolynomialTrie(const vector<string>& varNames, int mod);

    PolynomialTrie(const PolynomialTrie& other);
    PolynomialTrie& operator=(const PolynomialTrie& other);

    PolynomialTrie(PolynomialTrie&& other) noexcept = default;
    PolynomialTrie& operator=(PolynomialTrie&& other) noexcept = default;

    int varCount() const;
    const vector<string>& variableNames() const;
    int getModulus() const;

    void setVariableNames(const vector<string>& varNames);
    void setModulus(int mod);
    void clear();

    int normalizeCoef(int value) const;

    void addMonomial(int coef, const vector<int>& exponents);
    void setMonomial(int coef, const vector<int>& exponents);

    vector<Term> terms() const;
    void normalize();
    bool isZero() const;

    PolynomialTrie& operator+=(const PolynomialTrie& other);
    PolynomialTrie& operator-=(const PolynomialTrie& other);
    PolynomialTrie& operator*=(const PolynomialTrie& other);

    PolynomialTrie operator+(const PolynomialTrie& other) const;
    PolynomialTrie operator-(const PolynomialTrie& other) const;
    PolynomialTrie operator*(const PolynomialTrie& other) const;

    bool operator==(const PolynomialTrie &other) const;
    bool operator!=(const PolynomialTrie &other) const;

    vector<vector<int>> support() const;
    int evaluateAt(const vector<int>& point) const;
    int homeDegree() const;
    std::pair<PolynomialTrie, PolynomialTrie> splitByDegree(int degree) const;

    std::string monomialToString(const Term& t) const;
    std::string toString() const;
};

int fastPowMod(int a, int e, int mod);
int totalDegree(const std::vector<int>& exponents);