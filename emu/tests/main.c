#include <stdio.h>
#include "../src/cpu.h" 
#include <stdbool.h>

#define _CPUTEST(name) bool name()

#define TEST_CHECK(name) \
	do { \
		if (name()) { \
			printf("%-20s [\033[1;32m PASS \033[0m]\n", #name); \
		} else { \
			printf("%-20s [\033[1;31m FAIL \033[0m]\n", #name); \
	   	} \
	} while (0)

void dump_memory() {
	for (int i = 0; i < 9; i++) {
		printf("%d: %d\n", i, _cpu.memory[i]);
	}
}

_CPUTEST(subleq_test) {
	reset();    

	int8_t program[3] = { 0x03, 0x04, 0x06 };
	load_program(program, 3);

	_cpu.memory[3] = 7; 
	_cpu.memory[4] = 7; 

	run(1); 

	if (_cpu.memory[4] != 0) {
		printf("Value at address 4 is %d, expected 0\n", _cpu.memory[4]);
		return false;
	}

	if (_cpu.pc != 6) {
		printf("PC is %d, expected 6\n", _cpu.pc);
		return false;
	}

	return true;
}

_CPUTEST(subleq_test_no_branch) {
	reset();    

	int8_t program[3] = { 0x03, 0x04, 0x06 };
	load_program(program, 3);

	_cpu.memory[3] = 6; 
	_cpu.memory[4] = 14; 

	run(1); 

	if (_cpu.memory[4] != 8) {
		printf("Value at address 4 is %d, expected 0\n", _cpu.memory[4]);
		return false;
	}

	if (_cpu.pc != 3) {
		printf("PC is %d, expected 6\n", _cpu.pc);
		return false;
	}

	return true;
}

int main() {
	TEST_CHECK(subleq_test);
	TEST_CHECK(subleq_test_no_branch);
	return 0;
}
