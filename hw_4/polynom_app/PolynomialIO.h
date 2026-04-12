#pragma once

#include "Polynomial.h"

#include <ostream>
#include <string>
#include <vector>

struct JsonCommand {
    std::string op;
    std::vector<Coef> point;
    int degree = 0;
    std::string order;
};

struct JsonInputData {
    CoefDomain domain = CoefDomain::Zk;
    Coef modulus = 2;
    std::vector<std::string> variables;
    PolynomialTrie f;
    PolynomialTrie g;
    std::vector<JsonCommand> commands;
};

JsonInputData readJsonInput(const std::string& filename);

CoefDomain parseDomain(const std::string& s);
MonomialOrder parseOrder(const std::string& s);

void executeCommands(const JsonInputData& data, std::ostream& out);
void printSupport(const PolynomialTrie& f, std::ostream& out);
void printMultiDegree(const std::vector<int>& exponents, std::ostream& out);