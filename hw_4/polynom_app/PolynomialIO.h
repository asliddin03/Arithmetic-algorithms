#pragma once

#include "Polynomial.h"

#include <ostream>
#include <string>
#include <vector>

using std::ostream;
using std::string;
using std::vector;

enum class RingViewMode {
    AllVariables,
    ZAsCoefficient
};

struct JsonCommand {
    string op;
    vector<long long> point;
    int degree = 0;
};

template <class Poly>
struct JsonInputDataT {
    string domain;

    vector<string> variables;
    MonomialOrder order = MonomialOrder::Lex;

    RingViewMode ringViewMode = RingViewMode::AllVariables;
    string zVariableName = "z";

    Poly f;
    Poly g;
    vector<Poly> basis;
    vector<JsonCommand> commands;
};

RingViewMode parseRingViewMode(const string& s);
string ringViewModeToString(RingViewMode mode);

MonomialOrder parseOrder(const string& s);
string orderToString(MonomialOrder order);

template <class Poly>
JsonInputDataT<Poly> readJsonInputT(const string& filename);

template <class Poly>
MonomialView buildMonomialView(const JsonInputDataT<Poly>& data);

template <class Poly>
void printSupport(const Poly& f, ostream& out);

void printMultiDegree(const vector<int>& exponents, ostream& out);

template <class Poly>
void executeCommands(const JsonInputDataT<Poly>& data, ostream& out);