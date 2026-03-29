#include "PolynomialIO.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

int main() {
    try {
        const JsonInputData data = readJsonInput("input.json");

        std::ofstream out("output.txt");
        if (!out) {
            throw std::runtime_error("Cannot open output.txt");
        }

        executeCommands(data, out);
        return 0;
    } catch (const nlohmann::json::parse_error& ex) {
        std::ofstream out("output.txt");
        if (out) {
            out << "JSON parse error: " << ex.what() << "\n";
        } else {
            std::cerr << "JSON parse error: " << ex.what() << "\n";
        }
        return 1;
    } catch (const std::exception& ex) {
        std::ofstream out("output.txt");
        if (out) {
            out << "Error: " << ex.what() << "\n";
        } else {
            std::cerr << "Error: " << ex.what() << "\n";
        }
        return 1;
    }
}