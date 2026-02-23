#pragma once
#include <stdint.h>

typedef struct {
	int32_t memory[64000];
	int32_t total_cycles;
	int32_t pc;
} Cpu;

int32_t subleq();
int32_t step();
void run(int32_t cycles);
void load_program(int32_t *program, int size);
void reset();

extern Cpu _cpu;
