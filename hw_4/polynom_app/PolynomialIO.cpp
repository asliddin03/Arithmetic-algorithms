#include "PolynomialIO.h"

#include "CoeffTypes.h"

#include <complex>
#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

using std::ifstream;
using std::ostream;
using std::runtime_error;
using std::string;
using std::vector;
using json = nlohmann::json;

RingViewMode parseRingViewMode(const string& s) {
    if (s == "all_variables") {
        return RingViewMode::AllVariables;
    }
    if (s == "z_as_coefficient") {
        return RingViewMode::ZAsCoefficient;
    }

    throw runtime_error("Unknown ring mode: " + s);
}

string ringViewModeToString(RingViewMode mode) {
    switch (mode) {
        case RingViewMode::AllVariables:
            return "all_variables";
        case RingViewMode::ZAsCoefficient:
            return "z_as_coefficient";
    }

    return "unknown";
}

MonomialOrder parseOrder(const string& s) {
    if (s == "lex") {
        return MonomialOrder::Lex;
    }
    if (s == "grlex") {
        return MonomialOrder::GrLex;
    }
    if (s == "grevlex") {
        return MonomialOrder::GrevLex;
    }

    throw runtime_error("Unknown monomial order: " + s);
}

string orderToString(MonomialOrder order) {
    switch (order) {
        case MonomialOrder::Lex:
            return "lex";
        case MonomialOrder::GrLex:
            return "grlex";
        case MonomialOrder::GrevLex:
            return "grevlex";
    }

    return "unknown";
}

namespace {

template <class Coef>
Coef parseJsonCoef(const json& j);

template <>
long long parseJsonCoef<long long>(const json& j) {
    return j.get<long long>();
}

template <>
double parseJsonCoef<double>(const json& j) {
    return j.get<double>();
}

template <>
std::complex<double> parseJsonCoef<std::complex<double>>(const json& j) {
    if (j.is_number()) {
        return {j.get<double>(), 0.0};
    }

    if (!j.is_object()) {
        throw runtime_error("Complex coefficient must be a number or object {re, im}.");
    }

    double re = 0.0;
    double im = 0.0;

    if (j.contains("re")) {
        re = j.at("re").get<double>();
    }
    if (j.contains("im")) {
        im = j.at("im").get<double>();
    }

    return {re, im};
}

template <>
ModInt parseJsonCoef<ModInt>(const json& j) {
    if (!j.is_object()) {
        throw runtime_error("Zn coefficient must be object {value, mod}.");
    }

    if (!j.contains("value") || !j.contains("mod")) {
        throw runtime_error("Zn coefficient must contain value and mod.");
    }

    return ModInt(
        j.at("value").get<long long>(),
        j.at("mod").get<long long>()
    );
}

template <class Poly>
Poly buildPolynomial(
    const vector<string>& variables,
    const json& polyJson
) {
    if (!polyJson.is_array()) {
        throw runtime_error("Polynomial must be a JSON array.");
    }

    Poly poly(variables);
    using Coef = typename Poly::Coef;

    for (const auto& monomial : polyJson) {
        if (!monomial.is_object()) {
            throw runtime_error("Each monomial must be a JSON object.");
        }

        if (!monomial.contains("coef") || !monomial.contains("exponents")) {
            throw runtime_error("Monomial must contain coef and exponents.");
        }

        const Coef coef = parseJsonCoef<Coef>(monomial.at("coef"));
        const vector<int> exponents =
            monomial.at("exponents").get<vector<int>>();

        poly.addMonomial(coef, exponents);
    }

    return poly;
}

template <class Poly>
vector<Poly> buildBasis(
    const vector<string>& variables,
    const json& basisJson
) {
    if (!basisJson.is_array()) {
        throw runtime_error("basis must be a JSON array.");
    }

    vector<Poly> basis;
    basis.reserve(basisJson.size());

    for (const auto& polyJson : basisJson) {
        basis.push_back(buildPolynomial<Poly>(variables, polyJson));
    }

    return basis;
}

JsonCommand parseCommand(const json& cmdJson) {
    if (!cmdJson.is_object() || !cmdJson.contains("op")) {
        throw runtime_error("Command must be an object with field op.");
    }

    JsonCommand cmd;
    cmd.op = cmdJson.at("op").get<string>();

    if (cmdJson.contains("point")) {
        cmd.point = cmdJson.at("point").get<vector<long long>>();
    }

    if (cmdJson.contains("degree")) {
        cmd.degree = cmdJson.at("degree").get<int>();
    }

    return cmd;
}

template <class Coef>
vector<Coef> convertPoint(const vector<long long>& point);

template <>
vector<long long> convertPoint<long long>(const vector<long long>& point) {
    return point;
}

template <>
vector<double> convertPoint<double>(const vector<long long>& point) {
    vector<double> result;
    result.reserve(point.size());

    for (long long x : point) {
        result.push_back(static_cast<double>(x));
    }

    return result;
}

template <>
vector<std::complex<double>> convertPoint<std::complex<double>>(const vector<long long>& point) {
    vector<std::complex<double>> result;
    result.reserve(point.size());

    for (long long x : point) {
        result.push_back({static_cast<double>(x), 0.0});
    }

    return result;
}

template <>
vector<ModInt> convertPoint<ModInt>(const vector<long long>&) {
    throw runtime_error("Point evaluation for Zn is not enabled in this version.");
}

template <class Poly>
void printBasis(
    const vector<Poly>& basis,
    const MonomialView& view,
    ostream& out
) {
    if (basis.empty()) {
        out << "basis = empty\n";
        return;
    }

    out << "basis:\n";
    for (size_t i = 0; i < basis.size(); ++i) {
        out << "g" << (i + 1) << " = " << basis[i].toString(view) << "\n";
    }
}

} // namespace

template <class Poly>
JsonInputDataT<Poly> readJsonInputT(const string& filename) {
    ifstream in(filename);
    if (!in) {
        throw runtime_error("Cannot open input file: " + filename);
    }

    json j = json::parse(in);

    if (!j.is_object()) {
        throw runtime_error("Root JSON must be an object.");
    }

    if (!j.contains("variables") ||
        !j.contains("order") ||
        !j.contains("f") ||
        !j.contains("g") ||
        !j.contains("commands")) {
        throw runtime_error("JSON must contain variables, order, f, g, commands.");
    }

    JsonInputDataT<Poly> data;

    if (j.contains("domain")) {
        data.domain = j.at("domain").get<string>();
    }

    data.variables = j.at("variables").get<vector<string>>();
    data.order = parseOrder(j.at("order").get<string>());

    if (j.contains("ring_mode")) {
        data.ringViewMode = parseRingViewMode(j.at("ring_mode").get<string>());
    }

    if (j.contains("z_variable")) {
        data.zVariableName = j.at("z_variable").get<string>();
    }

    data.f = buildPolynomial<Poly>(data.variables, j.at("f"));
    data.g = buildPolynomial<Poly>(data.variables, j.at("g"));

    if (j.contains("basis")) {
        data.basis = buildBasis<Poly>(data.variables, j.at("basis"));
    }

    const json& commandsJson = j.at("commands");
    if (!commandsJson.is_array()) {
        throw runtime_error("commands must be an array.");
    }

    data.commands.reserve(commandsJson.size());
    for (const auto& cmdJson : commandsJson) {
        data.commands.push_back(parseCommand(cmdJson));
    }

    return data;
}

template <class Poly>
MonomialView buildMonomialView(const JsonInputDataT<Poly>& data) {
    MonomialView view;
    view.order = data.order;
    view.zIndex = -1;

    if (data.ringViewMode == RingViewMode::ZAsCoefficient) {
        int zIndex = -1;

        for (int i = 0; i < static_cast<int>(data.variables.size()); ++i) {
            if (data.variables[i] == data.zVariableName) {
                zIndex = i;
                break;
            }
        }

        if (zIndex < 0) {
            throw runtime_error("z_variable was not found in variables.");
        }

        view.zIndex = zIndex;
    }

    return view;
}

template <class Poly>
void printSupport(const Poly& f, ostream& out) {
    const auto supp = f.support();

    out << "supp(f):\n";
    if (supp.empty()) {
        out << "empty\n";
        return;
    }

    for (const auto& exps : supp) {
        out << "(";
        for (size_t i = 0; i < exps.size(); ++i) {
            out << exps[i];
            if (i + 1 != exps.size()) {
                out << ", ";
            }
        }
        out << ")\n";
    }
}

void printMultiDegree(const vector<int>& exponents, ostream& out) {
    out << "(";
    for (size_t i = 0; i < exponents.size(); ++i) {
        out << exponents[i];
        if (i + 1 != exponents.size()) {
            out << ", ";
        }
    }
    out << ")";
}

template <class Poly>
void executeCommands(const JsonInputDataT<Poly>& data, ostream& out) {
    using Ops = typename Poly::OpsType;
    using Coef = typename Poly::Coef;

    const Poly& f = data.f;
    const Poly& g = data.g;
    const vector<Poly>& basis = data.basis;

    const MonomialView view = buildMonomialView(data);
    const string orderName = orderToString(data.order);
    const string ringModeName = ringViewModeToString(data.ringViewMode);

    out << "order = " << orderName << "\n";
    out << "ring_mode = " << ringModeName << "\n\n";

    for (const auto& cmd : data.commands) {
        if (cmd.op == "print") {
            out << "f = " << f.toString() << "\n";
            out << "g = " << g.toString() << "\n";
        } else if (cmd.op == "print_ordered") {
            out << "f = " << f.toString(view) << "\n";
            out << "g = " << g.toString(view) << "\n";
        } else if (cmd.op == "leading_data") {
            out << "lm(f) = " << f.monomialOnlyToString(f.leadingMonomial(view)) << "\n";
            out << "lt(f) = " << f.monomialToString(f.leadingTerm(view)) << "\n";
            out << "lc(f) = " << Ops::toString(f.leadingCoefficient(view)) << "\n";
            out << "multideg(f) = ";
            printMultiDegree(f.multiDegree(view), out);
            out << "\n";
        } else if (cmd.op == "s_polynomial") {
            const Poly s = f.sPolynomial(g, view);
            out << "S(f, g) = " << s.toString(view) << "\n";
        } else if (cmd.op == "multideg_pair") {
            out << "multideg(f, g) = ";
            printMultiDegree(f.multiDegreePair(g, view), out);
            out << "\n";
        } else if (cmd.op == "multideg_s") {
            const Poly s = f.sPolynomial(g, view);
            out << "multideg(S(f, g)) = ";
            printMultiDegree(s.multiDegree(view), out);
            out << "\n";
        } else if (cmd.op == "add") {
            out << "f + g = " << (f + g).toString(view) << "\n";
        } else if (cmd.op == "subtract") {
            out << "f - g = " << (f - g).toString(view) << "\n";
        } else if (cmd.op == "multiply") {
            out << "f * g = " << (f * g).toString(view) << "\n";
        } else if (cmd.op == "support") {
            printSupport(f, out);
        } else if (cmd.op == "equal") {
            out << ((f == g) ? "f == g\n" : "f != g\n");
        } else if (cmd.op == "eval") {
            const vector<Coef> point = convertPoint<Coef>(cmd.point);
            out << "f(A) = " << Ops::toString(f.evaluateAt(point)) << "\n";
        } else if (cmd.op == "homogeneous_degree") {
            if (f.isZero()) {
                out << "homogeneous degree = 0\n";
            } else {
                const int d = f.homeDegree();
                if (d == -1) {
                    out << "not homogeneous\n";
                } else {
                    out << "homogeneous degree = " << d << "\n";
                }
            }
        } else if (cmd.op == "split_by_degree") {
            const auto parts = f.splitByDegree(cmd.degree);
            out << "h = " << parts.first.toString(view) << "\n";
            out << "g = " << parts.second.toString(view) << "\n";
        } else if (cmd.op == "print_basis") {
            printBasis(basis, view, out);
        } else if (cmd.op == "groebner_check") {
            printBasis(basis, view, out);
            out << (Poly::isGroebnerBasis(basis, view)
                        ? "This basis is a Groebner basis.\n"
                        : "This basis is NOT a Groebner basis.\n");
        } else if (cmd.op == "buchberger_basis") {
            const vector<Poly> gb = Poly::buchbergerBasis(basis, view);

            out << "Buchberger basis:\n";
            for (size_t i = 0; i < gb.size(); ++i) {
                out << "g" << (i + 1) << " = " << gb[i].toString(view) << "\n";
            }
        } else if (cmd.op == "buchberger_compare_views") {
            MonomialView allView;
            allView.order = data.order;
            allView.zIndex = -1;

            const MonomialView zView = buildMonomialView(data);

            out << "compare views:\n";
            out << "view 1 = all_variables\n";
            {
                const vector<Poly> gb1 = Poly::buchbergerBasis(basis, allView);
                out << "Buchberger basis in k[x1,...,xn,z]:\n";
                for (size_t i = 0; i < gb1.size(); ++i) {
                    out << "g" << (i + 1) << " = " << gb1[i].toString(allView) << "\n";
                }
            }

            out << "\nview 2 = " << ringModeName << "\n";
            {
                const vector<Poly> gb2 = Poly::buchbergerBasis(basis, zView);
                out << "Buchberger basis in k[z][x1,...,xn]:\n";
                for (size_t i = 0; i < gb2.size(); ++i) {
                    out << "g" << (i + 1) << " = " << gb2[i].toString(zView) << "\n";
                }
            }
        } else {
            throw runtime_error("Unknown operation: " + cmd.op);
        }

        out << "\n";
    }
}

/* explicit instantiations */

template JsonInputDataT<Polynomial<IntegerOps>>
readJsonInputT<Polynomial<IntegerOps>>(const string& filename);

template JsonInputDataT<Polynomial<RealOps>>
readJsonInputT<Polynomial<RealOps>>(const string& filename);

template JsonInputDataT<Polynomial<ComplexOps>>
readJsonInputT<Polynomial<ComplexOps>>(const string& filename);

template MonomialView
buildMonomialView<Polynomial<IntegerOps>>(const JsonInputDataT<Polynomial<IntegerOps>>& data);

template MonomialView
buildMonomialView<Polynomial<RealOps>>(const JsonInputDataT<Polynomial<RealOps>>& data);

template MonomialView
buildMonomialView<Polynomial<ComplexOps>>(const JsonInputDataT<Polynomial<ComplexOps>>& data);

template void
printSupport<Polynomial<IntegerOps>>(const Polynomial<IntegerOps>& f, ostream& out);

template void
printSupport<Polynomial<RealOps>>(const Polynomial<RealOps>& f, ostream& out);

template void
printSupport<Polynomial<ComplexOps>>(const Polynomial<ComplexOps>& f, ostream& out);

template void
executeCommands<Polynomial<IntegerOps>>(const JsonInputDataT<Polynomial<IntegerOps>>& data, ostream& out);

template void
executeCommands<Polynomial<RealOps>>(const JsonInputDataT<Polynomial<RealOps>>& data, ostream& out);

template void
executeCommands<Polynomial<ComplexOps>>(const JsonInputDataT<Polynomial<ComplexOps>>& data, ostream& out);