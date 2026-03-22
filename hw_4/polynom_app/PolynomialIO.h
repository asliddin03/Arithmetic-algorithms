#pragma once

#include "Polynomial.h"

#include <string>
#include <vector>

int readModulus();
std::vector<std::string> readVariableNames();
PolynomialTrie readPolynomial(const std::vector<std::string>& vars, int mod, const std::string& polyName);

void printMenu();
void printSupport(const PolynomialTrie& f);