#pragma once

#include "Polynomial.h"

#include <ostream>
#include <string>
#include <vector>

struct JsonCommand {
    std::string op;
    std::vector<int> point;
    int degree = 0;
};

struct JsonInputData {
    int modulus = 0;
    std::vector<std::string> variables;
    PolynomialTrie f;
    PolynomialTrie g;
    std::vector<JsonCommand> commands;
};

JsonInputData readJsonInput(const std::string& filename);
void executeCommands(const JsonInputData& data, std::ostream& out);
void printSupport(const PolynomialTrie& f, std::ostream& out);