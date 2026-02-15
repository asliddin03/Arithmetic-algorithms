#pragma once
#include <string>
#include "polynomial_utils.h"

using std::string;
using std::vector;

// Предел при x->A для R(x)=f(x)/g(x)
string limitAtA(vector<long double> f, vector<long double> g, long double A);
