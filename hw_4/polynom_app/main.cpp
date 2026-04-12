#include "PolynomialIO.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

#include <nlohmann/json.hpp>

int main() {
    try {
        const JsonInputData data = readJsonInput("input.json");

        std::ofstream out("output.txt");
        if (!out) {
            throw std::runtime_error("Cannot open output.txt");
        }

        executeCommands(data, out);
        std::cout << "OK\n";
        return 0;

    } catch (const nlohmann::json::parse_error& ex) {
        std::cerr << "JSON parse error: " << ex.what() << "\n";
        return 1;

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 3;
    }
}