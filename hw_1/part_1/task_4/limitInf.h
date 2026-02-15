#pragma once
#include <string>
#include "polynomial_utils.h"

using std::string;
using std::vector;
using std::pair;

// Пределы при x-> +∞ и x-> -∞ для R(x)=f(x)/g(x)
pair<string, string> limitInf(vector<long double> f, vector<long double> g);
