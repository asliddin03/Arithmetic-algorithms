#include "PolynomialIO.h"

#include "CoeffTypes.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include <nlohmann/json.hpp>

using std::ifstream;
using std::ofstream;
using std::runtime_error;
using std::string;
using json = nlohmann::json;

namespace {

    string readDomainFromJson(const string& filename) {
        ifstream in(filename);
        if (!in) {
            throw runtime_error("Cannot open input file: " + filename);
        }

        json j = json::parse(in);

        if (!j.is_object()) {
            throw runtime_error("Root JSON must be an object.");
        }

        if (!j.contains("domain")) {
            return "Z";
        }

        return j.at("domain").get<string>();
    }

    template <class Poly>
    int runTyped() {
        const auto data = readJsonInputT<Poly>("input.json");

        ofstream out("output.txt");
        if (!out) {
            throw runtime_error("Cannot open output.txt");
        }

        executeCommands(data, out);
        std::cout << "OK\n";
        return 0;
    }

} // namespace

int main() {
    try {
        const string domain = readDomainFromJson("input.json");

        if (domain == "Z") {
            return runTyped<Polynomial<IntegerOps>>();
        }

        if (domain == "R") {
            return runTyped<Polynomial<RealOps>>();
        }

        if (domain == "C") {
            return runTyped<Polynomial<ComplexOps>>();
        }

        throw runtime_error("Unsupported domain: " + domain);
    } catch (const nlohmann::json::parse_error& ex) {
        std::cerr << "JSON parse error: " << ex.what() << "\n";
        return 1;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 3;
    }
}