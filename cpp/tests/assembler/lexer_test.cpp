#include "../../lexer.hpp"
#include "../tester.hpp"
#include <fstream>

bool lexer_test_func(const std::string& test_name, const json& data) {
    std::string input = data["input"];
    std::vector<Token> output;
    try {
        output = tokenize(input);
    } catch (const std::exception& e) {
        std::cerr << "Lexer error in " << test_name << ": " << e.what() << std::endl;
        return false;
    }

    auto expected = data["expected"];
    if (output.size() != expected.size()) {
        std::cout << "Token count mismatch in " << test_name << ": Expected " << expected.size() << ", got " << output.size() << std::endl;
        return false;
    }

    for (size_t i = 0; i < output.size(); i++) {
        if (output[i].type != expected[i]["type"] || output[i].value != expected[i]["value"]) {
            std::cout << "Mismatch at index " << i << " in " << test_name << ": Expected " << expected[i]["type"] << ":" << expected[i]["value"] << ", got " << output[i].type << ":" << output[i].value << std::endl;
            return false;
        }
    }

    return true;
}

int main() {
    std::string data_path = "tests/assembler/lexer_data.json";
    std::ifstream file(data_path);
    if (!file.is_open()) {
        std::cerr << "Could not open data file: " << data_path << std::endl;
        return 1;
    }

    json test_data;
    file >> test_data;

    tester::run<json>("Lexer Tests", test_data, lexer_test_func);

    return 0;
}
