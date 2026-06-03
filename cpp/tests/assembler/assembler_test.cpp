#include "../../assembler.hpp"
#include "../tester.hpp"
#include <fstream>
#include <iostream>

bool pass1_test_func(const std::string& test_name, const json& data) {
    Pass1Result res = pass1(data["input"].get<std::string>());
    auto expected = data["expected"];

    if (res.pc != expected["pc"].get<int>()) {
        std::cout << "PC mismatch in " << test_name << ": Expected " << expected["pc"] << ", got " << res.pc << std::endl;
        return false;
    }

    if (expected.contains("symbols")) {
        for (auto it = expected["symbols"].begin(); it != expected["symbols"].end(); ++it) {
            std::string label = it.key();
            int val = it.value();
            if (res.symbols.count(label) == 0 || res.symbols[label] != val) {
                std::cout << "Symbol mismatch for " << label << " in " << test_name << ": Expected " << val;
                if (res.symbols.count(label)) std::cout << ", got " << res.symbols[label];
                else std::cout << " (not found)";
                std::cout << std::endl;
                return false;
            }
        }
    }

    if (expected.contains("macros")) {
        for (auto it = expected["macros"].begin(); it != expected["macros"].end(); ++it) {
            std::string macro_name = it.key();
            int argc = it.value()["argc"];
            if (res.macros.count(macro_name) == 0 || res.macros[macro_name].argc != argc) {
                std::cout << "Macro mismatch for " << macro_name << " in " << test_name << ": Expected argc " << argc;
                if (res.macros.count(macro_name)) std::cout << ", got " << res.macros[macro_name].argc;
                else std::cout << " (not found)";
                std::cout << std::endl;
                return false;
            }
        }
    }

    if (expected.contains("tokens")) {
        if (res.tokens.size() != expected["tokens"].size()) {
            std::cout << "Token size mismatch in " << test_name << ": Expected " << expected["tokens"].size() << ", got " << res.tokens.size() << std::endl;
            return false;
        }
        for (size_t i = 0; i < res.tokens.size(); i++) {
            if (res.tokens[i].type != expected["tokens"][i]["type"] || res.tokens[i].value != expected["tokens"][i]["value"]) {
                std::cout << "Token mismatch at " << i << " in " << test_name << ": Expected " << expected["tokens"][i]["type"] << ":" << expected["tokens"][i]["value"] << ", got " << res.tokens[i].type << ":" << res.tokens[i].value << std::endl;
                return false;
            }
        }
    }

    return true;
}

bool pass2_test_func(const std::string& test_name, const json& data) {
    try {
        std::vector<int> binary = assemble(data["input"].get<std::string>());
        auto expected = data["expected"];
        if (binary.size() != expected.size()) {
            std::cout << "Binary size mismatch in " << test_name << ": Expected " << expected.size() << ", got " << binary.size() << std::endl;
            return false;
        }
        for (size_t i = 0; i < binary.size(); i++) {
            if (binary[i] != expected[i].get<int>()) {
                std::cout << "Binary mismatch at " << i << " in " << test_name << ": Expected " << expected[i] << ", got " << binary[i] << std::endl;
                return false;
            }
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Assembly failed in " << test_name << ": " << e.what() << std::endl;
        return false;
    }
}

int main() {
    std::string pass1_path = "tests/assembler/pass1_data.json";
    std::ifstream file1(pass1_path);
    if (file1.is_open()) {
        json pass1_data;
        file1 >> pass1_data;
        tester::run<json>("Assembler Pass 1 Tests", pass1_data, pass1_test_func);
    } else {
        std::cerr << "Could not open data file: " << pass1_path << std::endl;
    }

    std::string pass2_path = "tests/assembler/pass2_data.json";
    std::ifstream file2(pass2_path);
    if (file2.is_open()) {
        json pass2_data;
        file2 >> pass2_data;
        tester::run<json>("Assembler Pass 2 Tests", pass2_data, pass2_test_func);
    } else {
        std::cerr << "Could not open data file: " << pass2_path << std::endl;
    }

    return 0;
}
