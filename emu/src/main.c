#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "cpu.h"

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("Usage: %s <binary_file>\n", argv[0]);
		return 1;
	}

	FILE *f = fopen(argv[1], "rb");
	if (!f) {
		perror("Failed to open file");
		return 1;
	}

	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

    // Limit size to CPU memory
    if (fsize > sizeof(int32_t) * 64000) {
        printf("Program too large\n");
        fclose(f);
        return 1;
    }

	int32_t *program = malloc(fsize);
	fread(program, 1, fsize, f);
	fclose(f);

	load_program(program, fsize);
    free(program);

    // Run for a large number of cycles/steps
	run(1000000); 

    // Dump memory with correct names
    int num_ints = fsize / sizeof(int32_t);
    printf("Final Memory State (Last 10 words):\n");
    for (int i = num_ints - 10; i < num_ints; i++) {
        int offset_from_end = num_ints - 1 - i;
        char *name = "";
        
        // Z is last (0)
        // COUNT is 1
        // ONE is 2
        // d is 3
        // c is 4
        // b is 5
        // a is 6
        
        if (offset_from_end == 6) name = " (a)";
        if (offset_from_end == 5) name = " (b)";
        if (offset_from_end == 4) name = " (c)";
        if (offset_from_end == 3) name = " (d)";
        if (offset_from_end == 2) name = " (ONE)";
        if (offset_from_end == 1) name = " (COUNT)";
        if (offset_from_end == 0) name = " (Z)";
        
        if (i >= 0) {
            printf("[%d]: %d%s\n", i, _cpu.memory[i], name);
        }
    }
    
	return 0;
}
