#include "../../emu/cpu.hpp"
#include "../tester.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

bool emu_test_func(const std::string& test_name, const json& data) {
    CPU cpu;
    std::vector<int> program = data["program"].get<std::vector<int>>();
    cpu.load_program(program);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    // Safety cycle limit
    int cycles = data.value("max_cycles", 100);
    cpu.run(cycles);

    std::cout.rdbuf(old);
    std::string output = buffer.str();

    bool success = true;

    // Check Output
    if (data.contains("expected_output")) {
        std::string expected_output = data["expected_output"];
        if (output != expected_output) {
            std::cout << "Output mismatch in " << test_name << ": Expected " << expected_output << ", got " << output << std::endl;
            success = false;
        }
    }

    // Check PC
    if (data.contains("expected_pc")) {
        int expected_pc = data["expected_pc"];
        if (cpu.pc != expected_pc) {
            std::cout << "PC mismatch in " << test_name << ": Expected " << expected_pc << ", got " << cpu.pc << std::endl;
            success = false;
        }
    }

    // Check Memory
    if (data.contains("expected_memory")) {
        auto expected_memory = data["expected_memory"];
        for (auto it = expected_memory.begin(); it != expected_memory.end(); ++it) {
            int addr = std::stoi(it.key());
            int val = it.value();
            if ((size_t)addr >= cpu.memory.size() || cpu.memory[addr] != val) {
                std::cout << "Memory mismatch at address " << addr << " in " << test_name << ": Expected " << val;
                if ((size_t)addr < cpu.memory.size()) std::cout << ", got " << cpu.memory[addr];
                else std::cout << " (out of bounds)";
                std::cout << std::endl;
                success = false;
            }
        }
    }

    return success;
}

int main() {
    std::string data_path = "tests/emu/emu_test_data.json";
    std::ifstream file(data_path);
    if (!file.is_open()) {
        std::cerr << "Could not open data file: " << data_path << std::endl;
        return 1;
    }

    json test_data;
    file >> test_data;

    tester::run<json>("Emulator Tests", test_data, emu_test_func);

    return 0;
}
