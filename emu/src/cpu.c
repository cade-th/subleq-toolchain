#include "cpu.h"
#include <string.h>
#include <stdio.h>

Cpu _cpu = { 0 };

// subtract and branch if less than or equal
// every address in memory is both a value being operated on and a pointer to another place in memory

int32_t subleq() {
    if (_cpu.pc < 0 || _cpu.pc + 2 >= 64000) return 0; // Halt or invalid PC

    int32_t addr_a = _cpu.memory[_cpu.pc];
    int32_t addr_b = _cpu.memory[_cpu.pc + 1];
    int32_t addr_c = _cpu.memory[_cpu.pc + 2];
    
    int32_t val_a, val_b;

    // Debug: output PC and instruction
    // printf("PC: %d [%d %d %d]\n", _cpu.pc, addr_a, addr_b, addr_c);

    // Handle Input (Address -1 in A)
    if (addr_a == -1) {
        val_a = getchar();
    } else {
        if (addr_a < 0 || addr_a >= 64000) return 0; // Invalid address
        val_a = _cpu.memory[addr_a];
    }

    // Handle Output (Address -1 in B)
    if (addr_b == -1) {
        putchar((char)val_a);
        fflush(stdout);
        
        int32_t result = -val_a;
        if (result <= 0) {
            _cpu.pc = addr_c;
        } else {
            _cpu.pc += 3;
        }
        return 1;
    } else {
        if (addr_b < 0 || addr_b >= 64000) return 0; // Invalid address
        val_b = _cpu.memory[addr_b];
        int32_t result = val_b - val_a;
        _cpu.memory[addr_b] = result;
        
        if (result <= 0) {
            _cpu.pc = addr_c;
        } else {
            _cpu.pc += 3;
        }
    }

    return 1;
}

int32_t step() {
	return subleq();
}

void run(int32_t cycles) {
	while (_cpu.total_cycles < cycles) {
		int32_t res = step();
        if (res == 0) break; // Halt
        _cpu.total_cycles += res;
	}
}

void reset() {
	_cpu.total_cycles = 0;
	_cpu.pc = 0;
	// do we need to zero out memory?
};

void load_program(int32_t *program, int size) {
	memcpy(_cpu.memory, program, size);
}


