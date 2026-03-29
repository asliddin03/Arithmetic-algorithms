#include "PolynomialIO.h"

#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {

PolynomialTrie buildPolynomial(
    const std::vector<std::string>& variables,
    int modulus,
    const json& polyJson
) {
    if (!polyJson.is_array()) {
        throw std::runtime_error("Polynomial must be a JSON array.");
    }

    PolynomialTrie poly(variables, modulus);

    for (const auto& monomial : polyJson) {
        if (!monomial.is_object()) {
            throw std::runtime_error("Each monomial must be a JSON object.");
        }

        if (!monomial.contains("coef") || !monomial.contains("exponents")) {
            throw std::runtime_error("Monomial must contain coef and exponents.");
        }

        const int coef = monomial.at("coef").get<int>();
        const std::vector<int> exponents = monomial.at("exponents").get<std::vector<int>>();

        poly.addMonomial(coef, exponents);
    }

    return poly;
}

JsonCommand parseCommand(const json& cmdJson) {
    if (!cmdJson.is_object()) {
        throw std::runtime_error("Command must be a JSON object.");
    }

    if (!cmdJson.contains("op")) {
        throw std::runtime_error("Command must contain op.");
    }

    JsonCommand cmd;
    cmd.op = cmdJson.at("op").get<std::string>();

    if (cmdJson.contains("point")) {
        cmd.point = cmdJson.at("point").get<std::vector<int>>();
    }

    if (cmdJson.contains("degree")) {
        cmd.degree = cmdJson.at("degree").get<int>();
    }

    return cmd;
}

} // namespace

JsonInputData readJsonInput(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) {
        throw std::runtime_error("Cannot open input file: " + filename);
    }

    json j = json::parse(in);

    if (!j.is_object()) {
        throw std::runtime_error("Root JSON must be an object.");
    }

    if (!j.contains("modulus") || !j.contains("variables") ||
        !j.contains("f") || !j.contains("g") || !j.contains("commands")) {
        throw std::runtime_error("JSON must contain modulus, variables, f, g, commands.");
    }

    JsonInputData data;
    data.modulus = j.at("modulus").get<int>();
    data.variables = j.at("variables").get<std::vector<std::string>>();

    data.f = buildPolynomial(data.variables, data.modulus, j.at("f"));
    data.g = buildPolynomial(data.variables, data.modulus, j.at("g"));

    const json& commandsJson = j.at("commands");
    if (!commandsJson.is_array()) {
        throw std::runtime_error("commands must be an array.");
    }

    for (const auto& cmdJson : commandsJson) {
        data.commands.push_back(parseCommand(cmdJson));
    }

    return data;
}

void printSupport(const PolynomialTrie& f, std::ostream& out) {
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

void executeCommands(const JsonInputData& data, std::ostream& out) {
    const PolynomialTrie& f = data.f;
    const PolynomialTrie& g = data.g;

    for (const auto& cmd : data.commands) {
        if (cmd.op == "print") {
            out << "f = " << f.toString() << "\n";
            out << "g = " << g.toString() << "\n";
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
        } else {
            throw std::runtime_error("Unknown operation: " + cmd.op);
        }

        out << "\n";
    }
}