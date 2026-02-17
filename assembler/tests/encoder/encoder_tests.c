#include "encoder_tests.h"
#include <stdio.h>
#include <stdint.h>
#include "../../src/encoder/encoder.h"
#include "../../src/parser/parser.h"
#include <string.h>

static int32_t input_1[] = {
	0, 1, 2
};

static int32_t expected_1[] = {
	0, 1, 2
};

static int32_t input_2[] = {
	0, 1, 2,
	3, 4, 5
};

static int32_t expected_2[] = {
	0, 1, 2,
	3, 4, 5
};


static	bool encoder_test(
	int test_num,
	int32_t expected[],
	int32_t input[],
	int num_ints
) {
	int size = num_ints; // Encoder now takes raw count of ints if we want, or instruction count? 
    // Wait, encode takes "instruction_cnt". In new model, is that number of INTs or number of 3-word blocks?
    // My encode implementation loop: for (int i=0; i < instruction_cnt; i++) { ARRAY_PUSH(output, self->input[i]); }
    // So "instruction_cnt" is actually "int_cnt" (element count).
    
	Encoder encoder = encoder_init(input);
	int32_t *output = encode(&encoder, size);

	// compare shit
	// how to avoid num_bytes?
	if (memcmp(output, expected, num_ints * sizeof(int32_t)) != 0) {
		printf("Test #%d, some byte is wrong\n", test_num);	
		return false;
	}
	
	return true;
}

static bool encoder() {
	return encoder_test(1, expected_1,input_1, 3) && encoder_test(2, expected_2,input_2, 6);
}

void run_encoder_tests() {
	TEST_CHECK(encoder);
}
