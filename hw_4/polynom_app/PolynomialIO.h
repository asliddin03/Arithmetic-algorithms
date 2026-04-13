#pragma once

#include "Polynomial.h"

#include <ostream>
#include <string>
#include <vector>

struct JsonCommand {
    std::string op;
    std::vector<Coef> point;
    int degree = 0;
};

struct JsonInputData {
    std::vector<std::string> variables;
    MonomialOrder order = MonomialOrder::Lex;
    Polynomial f;
    Polynomial g;
    std::vector<Polynomial> basis;
    std::vector<JsonCommand> commands;
};

JsonInputData readJsonInput(const std::string& filename);

MonomialOrder parseOrder(const std::string& s);
std::string orderToString(MonomialOrder order);

void executeCommands(const JsonInputData& data, std::ostream& out);

void printSupport(const Polynomial& f, std::ostream& out);
void printMultiDegree(const std::vector<int>& exponents, std::ostream& out);