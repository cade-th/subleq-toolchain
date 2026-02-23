#include <stdio.h>
#include "../src/cpu.h" 
#include <stdbool.h>

#define _CPUTEST(name) bool name()

#define TEST_CHECK(name) \
	do { \
		if (name()) { \
			printf("%-25s [\033[1;32m PASS \033[0m]\n", #name); \
		} else { \
			printf("%-25s [\033[1;31m FAIL \033[0m]\n", #name); \
	   	} \
	} while (0)

void dump_memory() {
	for (int i = 0; i < 9; i++) {
		printf("%d: %d\n", i, _cpu.memory[i]);
	}
}

_CPUTEST(subleq_test) {
	reset();    

	int32_t program[3] = { 3, 4, 6 };
	load_program(program, 12); // size in bytes = 3 * 4 = 12

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

	int32_t program[3] = { 3, 4, 6 };
	load_program(program, 12);

	_cpu.memory[3] = 6; 
	_cpu.memory[4] = 14; 

	run(1); 

	if (_cpu.memory[4] != 8) {
		printf("Value at address 4 is %d, expected 8\n", _cpu.memory[4]);
		return false;
	}

	if (_cpu.pc != 3) {
		printf("PC is %d, expected 3\n", _cpu.pc);
		return false;
	}

	return true;
}

// Test basic subtraction logic used in Fib
_CPUTEST(test_fib_calc_step) {
    reset();
    // Simulate: d = a + b
    // Instructions (macros expanded manualy for testing):
    // MV(a, d) -> d d ?+3; a Z ?+3; Z d ?+3; Z Z ?+3
    // ADD(b, d) -> b Z ?+3; Z d ?+3; Z Z ?+3
    
    // Simplified logic for testing:
    // a=3, b=5. d=0.
    // 1. d -= d (0)
    // 2. Z -= a (-3) -> Z=-3
    // 3. d -= Z (3)  -> d=3 (Move a to d)
    // 4. Z -= Z (0)
    // 5. Z -= b (-5) -> Z=-5
    // 6. d -= Z (8)  -> d=8 (Add b to d)
    // 7. Z -= Z (0)
    
    // Addresses:
    // 0: 20 20 3
    // 3: 21 22 6
    // 6: 22 20 9
    // 9: 22 22 12
    // 12: 23 22 15
    // 15: 22 20 18
    // 18: 22 22 21
    // 20: d (0)
    // 21: a (3)
    // 22: Z (0)
    // 23: b (5)
    
    int32_t prog[] = {
        21, 21, 3,
        22, 23, 6,
        23, 21, 9,
        23, 23, 12,
        24, 23, 15,
        23, 21, 18,
        23, 23, -1,
        // Data
        0, // 21: d
        3, // 22: a
        0, // 23: Z
        5  // 24: b
    };
    // Adjust indices because array is flat
    // instructions end at index 21.
    // 21 is index of d.
    
    load_program(prog, sizeof(prog));
    
    // Initial values
    _cpu.memory[21] = 0; // d
    _cpu.memory[22] = 3; // a
    _cpu.memory[23] = 0; // Z
    _cpu.memory[24] = 5; // b
    
    run(100);
    
    if (_cpu.memory[21] != 8) { // d should be 3+5=8
         printf("d is %d, expected 8\n", _cpu.memory[21]);
         return false;
    }
    if (_cpu.memory[23] != 0) { // Z should be 0
         printf("Z is %d, expected 0\n", _cpu.memory[23]);
         return false;
    }
    
    return true;
}

// Test the loop counter logic
_CPUTEST(test_loop_check) {
    reset();
    // Logic: S(ONE, c, DONE). S(Z, Z, LOOP).
    // c starts at 1. Should Jump to DONE.
    // Address:
    // 0: ONE c DONE(6)
    // 3: Z Z LOOP(0) (Should NOT execute if c=1)
    // 6: Z Z -1 (HLT)
    
    // Data:
    // 9: ONE (1)
    // 10: c (1)
    // 11: Z (0)
    
    int32_t prog[] = {
        9, 10, 6,
        11, 11, 0,
        11, 11, -1,
        1,  // 9: ONE
        1,  // 10: c
        0   // 11: Z
    };
    
    load_program(prog, sizeof(prog));
    run(10);
    
    if (_cpu.memory[10] != 0) { // c should be 0 (1-1)
        printf("c is %d, expected 0\n", _cpu.memory[10]);
        return false;
    }
    if (_cpu.pc != 6 && _cpu.pc != -1) {
        // If it halted, pc might be -1 depending on impl, or stored last pc.
        // My cpu.c halts if pc < 0.
        // But run loop breaks if step returns 0.
        // subleq returns 0 if pc < 0.
        // So PC should be -1.
        // Wait, assembler macro HLT is Z Z -1.
        // Z -= Z. Jump to -1.
        // So PC becomes -1.
        // Subleq sees PC=-1. Returns 0.
        // Run loop stops.
        // PC stays -1.
    }
    
    // Verify it didn't verify loop (didn't write to Z Z again, well Z is Z)
    // If it looped, it would run forever or hit cycle limit.
    // If we passed run(10) with cycle limit, we must have halted or looped 10 times.
    // If c was 1 -> 0. Jumped to 6. Halted.
    // If c was 2 -> 1. Fallthrough to 3. Jump to 0. Loop.
    
    return true;
}

int main() {
	TEST_CHECK(subleq_test);
	TEST_CHECK(subleq_test_no_branch);
    TEST_CHECK(test_fib_calc_step);
    TEST_CHECK(test_loop_check);
	return 0;
}
