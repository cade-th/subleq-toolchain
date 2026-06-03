#include "cpu.hpp"
#include <iostream>

int CPU::subleq() {
    if (pc < 0 || (size_t)pc + 2 >= memory.size()) {
        return 0;
    }

    int addr_A = memory[pc];
    int addr_B = memory[pc + 1];
    int addr_C = memory[pc + 2];

    if (addr_B == -1) {
        if (addr_A >= 0 && (size_t)addr_A < memory.size()) {
            int val_a = memory[addr_A];
            std::cout << (char)val_a << std::flush;

            if (-val_a <= 0) {
                pc = addr_C;
            } else {
                pc += 3;
            }
        } else {
            return 0; // Invalid address
        }
    } else {
        if (addr_A >= 0 && (size_t)addr_A < memory.size() &&
            addr_B >= 0 && (size_t)addr_B < memory.size()) {
            int val_a = memory[addr_A];
            int val_b = memory[addr_B];
            int result = val_b - val_a;
            memory[addr_B] = result;

            if (result <= 0) {
                pc = addr_C;
            } else {
                pc += 3;
            }
        } else {
            return 0; // Invalid address
        }
    }

    return 1;
}

void CPU::step() {
    total_cycles += subleq();
}

void CPU::run(int cycles) {
    while (total_cycles < cycles && pc >= 0) {
        step();
    }
}

void CPU::load_program(const std::vector<int>& input_program) {
    memory = input_program;
    pc = 0;
    total_cycles = 0;
}

void CPU::reset() {
    pc = 0;
    total_cycles = 0;
}
