#include "parser_tests.h"
#include "../../src/parser/parser.h"
#include <stdbool.h>
#include <string.h>


static Token subleq_1[] = {
	{"0", INT},
	{"1", INT},
	{"3", INT},
	{" ", Eof}
};

static ParsedInstruction expected_1[] = {
	{"subleq", "0", "1", "3", 0},
};

static Token subleq_2[] = {
	{"0", INT},
	{"1", INT},
	{"2", INT},
	{"3", INT},
	{"4", INT},
	{"5", INT},
	{" ", Eof}
};

static ParsedInstruction expected_2[] = {
	{"subleq", "0", "1", "2", 0},
	{"subleq", "3", "4", "5", 0},
};


static Token subleq_3[] = {
	{"#", HASH},
	{"define", SYMBOL},
	{"VAR", SYMBOL},
	{"7", INT},	
	{"START", SYMBOL},
	{":", COLON},
	{"10", INT},
	{"10", INT},
	{"3", INT},
	{"10", INT},
	{"12", INT},
	{"3", INT},
	{" ", Eof}
};

static ParsedInstruction expected_3[] = {

};


static bool parser_test(
	int test_num,
	Token *input_tokens, 
	ParsedInstruction expected_output[], 
	int expected_size
	) {
	Parser parser = parser_init(input_tokens);	

	ParsedInstruction *output = parse(&parser);	

	for (int i=0; i < expected_size; i++) {
		if (strcmp(output[i].label, expected_output[i].label) != 0) {
			printf("Wrong label for index %d on test number %d\n", i, test_num); return false;
		}
		if (strcmp(output[i].op_a, expected_output[i].op_a) != 0) {
			printf("Wrong op_a for index %d on test number %d\n", i, test_num); return false;
		}
		if (strcmp(output[i].op_b, expected_output[i].op_b) != 0) {
			printf("Wrong op_b for index %d on test number %d\n", i, test_num); return false;
		}
		if (strcmp(output[i].op_c, expected_output[i].op_c) != 0) {
			printf("Wrong op_c for index %d on test number %d\n", i, test_num); return false;
		}
		if (output[i].address != expected_output[i].address) {
			printf("Wrong starting address for index %d on test number %d,\n", i, test_num); return false;
		}
	}
	
	return true;
}

// need this because the macro for pretty test colors wont take arguments for functions
static bool parser() {
	return parser_test(1,subleq_1, expected_1, 1) && parser_test(2,subleq_2, expected_2, 2);
}


void run_parser_tests() {
	TEST_CHECK(parser);
}
