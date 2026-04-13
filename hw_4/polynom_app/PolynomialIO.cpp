#include "PolynomialIO.h"

#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {

Polynomial buildPolynomial(
    const std::vector<std::string>& variables,
    const json& polyJson
) {
    if (!polyJson.is_array()) {
        throw std::runtime_error("Polynomial must be a JSON array.");
    }

    Polynomial poly(variables);

    for (const auto& monomial : polyJson) {
        if (!monomial.is_object()) {
            throw std::runtime_error("Each monomial must be a JSON object.");
        }

        if (!monomial.contains("coef") || !monomial.contains("exponents")) {
            throw std::runtime_error("Monomial must contain coef and exponents.");
        }

        const Coef coef = monomial.at("coef").get<Coef>();
        const std::vector<int> exponents =
            monomial.at("exponents").get<std::vector<int>>();

        poly.addMonomial(coef, exponents);
    }

    return poly;
}

std::vector<Polynomial> buildBasis(
    const std::vector<std::string>& variables,
    const json& basisJson
) {
    if (!basisJson.is_array()) {
        throw std::runtime_error("basis must be a JSON array.");
    }

    std::vector<Polynomial> basis;
    basis.reserve(basisJson.size());

    for (const auto& polyJson : basisJson) {
        basis.push_back(buildPolynomial(variables, polyJson));
    }

    return basis;
}

JsonCommand parseCommand(const json& cmdJson) {
    if (!cmdJson.is_object() || !cmdJson.contains("op")) {
        throw std::runtime_error("Command must be an object with field op.");
    }

    JsonCommand cmd;
    cmd.op = cmdJson.at("op").get<std::string>();

    if (cmdJson.contains("point")) {
        cmd.point = cmdJson.at("point").get<std::vector<Coef>>();
    }

    if (cmdJson.contains("degree")) {
        cmd.degree = cmdJson.at("degree").get<int>();
    }

    return cmd;
}

void printBasis(
    const std::vector<Polynomial>& basis,
    MonomialOrder order,
    std::ostream& out
) {
    if (basis.empty()) {
        out << "basis = empty\n";
        return;
    }

    out << "basis:\n";
    for (size_t i = 0; i < basis.size(); ++i) {
        out << "g" << (i + 1) << " = " << basis[i].toString(order) << "\n";
    }
}

} // namespace

MonomialOrder parseOrder(const std::string& s) {
    if (s == "lex") {
        return MonomialOrder::Lex;
    }
    if (s == "grlex") {
        return MonomialOrder::GrLex;
    }
    if (s == "grevlex") {
        return MonomialOrder::GrevLex;
    }

    throw std::runtime_error("Unknown monomial order: " + s);
}

std::string orderToString(MonomialOrder order) {
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

JsonInputData readJsonInput(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) {
        throw std::runtime_error("Cannot open input file: " + filename);
    }

    json j = json::parse(in);

    if (!j.is_object()) {
        throw std::runtime_error("Root JSON must be an object.");
    }

    if (!j.contains("variables") ||
        !j.contains("order") ||
        !j.contains("f") ||
        !j.contains("g") ||
        !j.contains("commands")) {
        throw std::runtime_error("JSON must contain variables, order, f, g, commands.");
    }

    JsonInputData data;
    data.variables = j.at("variables").get<std::vector<std::string>>();
    data.order = parseOrder(j.at("order").get<std::string>());
    data.f = buildPolynomial(data.variables, j.at("f"));
    data.g = buildPolynomial(data.variables, j.at("g"));

    if (j.contains("basis")) {
        data.basis = buildBasis(data.variables, j.at("basis"));
    }

    const json& commandsJson = j.at("commands");
    if (!commandsJson.is_array()) {
        throw std::runtime_error("commands must be an array.");
    }

    data.commands.reserve(commandsJson.size());
    for (const auto& cmdJson : commandsJson) {
        data.commands.push_back(parseCommand(cmdJson));
    }

    return data;
}

void printSupport(const Polynomial& f, std::ostream& out) {
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

void printMultiDegree(const std::vector<int>& exponents, std::ostream& out) {
    out << "(";
    for (size_t i = 0; i < exponents.size(); ++i) {
        out << exponents[i];
        if (i + 1 != exponents.size()) {
            out << ", ";
        }
    }
    out << ")";
}

void executeCommands(const JsonInputData& data, std::ostream& out) {
    const Polynomial& f = data.f;
    const Polynomial& g = data.g;
    const std::vector<Polynomial>& basis = data.basis;
    const MonomialOrder order = data.order;
    const std::string orderName = orderToString(order);

    for (const auto& cmd : data.commands) {
        if (cmd.op == "print") {
            out << "f = " << f.toString() << "\n";
            out << "g = " << g.toString() << "\n";
        } else if (cmd.op == "print_ordered") {
            out << "order = " << orderName << "\n";
            out << "f = " << f.toString(order) << "\n";
            out << "g = " << g.toString(order) << "\n";
        } else if (cmd.op == "leading_data") {
            out << "order = " << orderName << "\n";
            out << "lm(f) = " << f.monomialOnlyToString(f.leadingMonomial(order)) << "\n";
            out << "lt(f) = " << f.monomialToString(f.leadingTerm(order)) << "\n";
            out << "lc(f) = " << f.leadingCoefficient(order) << "\n";
            out << "multideg(f) = ";
            printMultiDegree(f.multiDegree(order), out);
            out << "\n";
        } else if (cmd.op == "s_polynomial") {
            const Polynomial s = f.sPolynomial(g, order);
            out << "order = " << orderName << "\n";
            out << "S(f, g) = " << s.toString(order) << "\n";
        } else if (cmd.op == "multideg_pair") {
            out << "order = " << orderName << "\n";
            out << "multideg(f, g) = ";
            printMultiDegree(f.multiDegreePair(g, order), out);
            out << "\n";
        } else if (cmd.op == "multideg_s") {
            const Polynomial s = f.sPolynomial(g, order);
            out << "order = " << orderName << "\n";
            out << "multideg(S(f, g)) = ";
            printMultiDegree(s.multiDegree(order), out);
            out << "\n";
        } else if (cmd.op == "add") {
            out << "f + g = " << (f + g).toString() << "\n";
        } else if (cmd.op == "subtract") {
            out << "f - g = " << (f - g).toString() << "\n";
        } else if (cmd.op == "multiply") {
            out << "f * g = " << (f * g).toString() << "\n";
        } else if (cmd.op == "support") {
            printSupport(f, out);
        } else if (cmd.op == "equal") {
            out << ((f == g) ? "f == g\n" : "f != g\n");
        } else if (cmd.op == "eval") {
            out << "f(A) = " << f.evaluateAt(cmd.point) << "\n";
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
            const auto [h, rest] = f.splitByDegree(cmd.degree);
            out << "h = " << h.toString() << "\n";
            out << "g = " << rest.toString() << "\n";
        } else if (cmd.op == "print_basis") {
            out << "order = " << orderName << "\n";
            printBasis(basis, order, out);
        } else if (cmd.op == "groebner_check") {
            out << "order = " << orderName << "\n";
            printBasis(basis, order, out);
            out << (Polynomial::isGroebnerBasis(basis, order)
                        ? "This basis is a Groebner basis.\n"
                        : "This basis is NOT a Groebner basis.\n");
        } else {
            throw std::runtime_error("Unknown operation: " + cmd.op);
        }

        out << "\n";
    }
}