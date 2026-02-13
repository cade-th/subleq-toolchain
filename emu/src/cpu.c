#include "cpu.h"
#include <string.h>

Cpu _cpu = { 0 };

// subtract and branch if less than or equal
// every address in memory is both a value being operated on and a pointer to another place in memory

int32_t subleq() {
    int8_t addr_a = _cpu.memory[_cpu.pc];
    int8_t addr_b = _cpu.memory[_cpu.pc + 1];
    int8_t addr_c = _cpu.memory[_cpu.pc + 2];
    int8_t val_a = _cpu.memory[addr_a];
    int8_t val_b = _cpu.memory[addr_b];

    int8_t result = val_b - val_a;
    _cpu.memory[addr_b] = val_b - val_a;

    if (result <= 0) {
        _cpu.pc = addr_c;
    } else {
        _cpu.pc += 3;
    }

    return 1;
}

int32_t step() {
	int8_t cycles = subleq();
	return cycles;
}

void run(int32_t cycles) {
	while (_cpu.total_cycles < cycles) {
		_cpu.total_cycles += step();
	}
}

void reset() {
	_cpu.total_cycles = 0;
	_cpu.pc = 0;
	// do we need to zero out memory?
};

void load_program(int8_t *program, int size) {
	memcpy(_cpu.memory, program, size);
}


