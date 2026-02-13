#include "encoder_tests.h"
#include <stdio.h>
#include <stdint.h>
#include "../../src/encoder/encoder.h"
#include "../../src/parser/parser.h"
#include <string.h>

static ParsedInstruction input_1[] = {
	{"subleq", "0", "1", "2", 0}
};

static uint8_t expected_1[] = {
	0, 1, 2
};

static ParsedInstruction input_2[] = {
	{"subleq", "0", "1", "2", 0},
	{"subleq", "3", "4", "5", 0}
};

static uint8_t expected_2[] = {
	0, 1, 2,
	3, 4, 5
};


bool encoder_test(
	int test_num,
	uint8_t expected[],
	ParsedInstruction input[],
	int num_bytes
) {
	int size = num_bytes / 3;
	Encoder encoder = encoder_init(input);
	uint8_t *output = encode(&encoder, size);

	// compare shit
	// how to avoid num_bytes?
	if (memcmp(output, expected, num_bytes) != 0) {
		printf("Test #%d, some byte is wrong\n", test_num);	
		return false;
	}
	
	return true;
}

bool encoder() {
	return encoder_test(1, expected_1,input_1, 3) && encoder_test(2, expected_2,input_2, 6);
}

void run_encoder_tests() {
	TEST_CHECK(encoder);
}
