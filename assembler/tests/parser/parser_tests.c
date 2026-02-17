#include "parser_tests.h"
#include "../../src/parser/parser.h"
#include "../lexer/lexer_tests.h"
#include <stdbool.h>
#include "../../src/util/io.h"
#include <string.h>


static Token subleq_1[] = {
	{"0", INT},
	{"1", INT},
	{"3", INT},
	{" ", Eof}
};

static ParsedInstruction expected_1[] = {
	{"subleq", 0, 1, 3, 0},
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
	{"subleq", 0, 2, 2, 0},
	{"subleq", 3, 4, 5, 0},
};


static Token subleq_3[] = {
	{"#", HASH},
	{"define", SYMBOL},
	{"VAR", SYMBOL},
	{"7", INT},	
	{"START", SYMBOL},
	{"10", INT},
	{"10", INT},
	{"3", INT},
	{"10", INT},
	{"12", INT},
	{"3", INT},
	{" ", Eof}
};

static Token expected_firstPass_1[] = {
	{"34", INT},
	{"5", INT},
	{"3", INT},
	{"23", INT},
	{"5", INT},
	{"LOOP", SYMBOL},
	{"45", INT},
	{"3", INT},
	{"6", INT},
	{" ", Eof}
};

static ParsedInstruction expected_second_pass_1[] = {
	{"subleq", 34, 5, 3, 0},
	{"subleq", 23, 5, 6, 3},
	{"subleq", 45, 3, 6, 6},
};

static bool parser_passOne_test(
	int test_num,
	char *path,
	Token *expected,
	int expected_size
) {
	File file = io_file_read(path);

	Lexer lexer = lexer_init(file.data); Token *tokens = lex(&lexer);
	Parser parser = parser_init(tokens);

	Token *output = parser_pass_one(&parser);
	
	// compare the token output
	for (int i=0; i < expected_size; i++) {
		if (!compare_tokens(i, expected[i], output[i])) {
			return false;
		}
	}

	// compare the symbol table output
	ST_Entry *symbol_1 = HASH_FIND(parser.symbol_table, "START", ST_Entry);
	ST_Entry *symbol_2 = HASH_FIND(parser.symbol_table, "LOOP", ST_Entry);

	
	// Check if the correct insertions to the Symbol Table are done
	if (strcmp(symbol_1->key, "START") != 0) {
		printf("Wrong key for START\n");
		return false; 
	}
	if (strcmp(symbol_2->key, "LOOP") != 0) {
		printf("Wrong key for LOOP\n");
		return false; 
	}

	// check the address of the labels
	if (symbol_1->address != 0) {
		printf("Wrong address for START\n");
		return false;
	}
	
	if (symbol_2->address != 6) {
		printf("Wrong address for LOOP, got %x\n", symbol_2->address);
		return false;
	}

	return true;
}

static bool parser_passTwo_test(
	int test_num,
	char *path,
	ParsedInstruction expected_output[], 
	int expected_size
	) {
	File file = io_file_read(path);

	Lexer lexer = lexer_init(file.data); Token *tokens = lex(&lexer);
	Parser parser = parser_init(tokens);

	Token *one = parser_pass_one(&parser);
	ParsedInstruction *output = parser_pass_two(&parser, one);

	for (int i=0; i < expected_size; i++) {
		if (strcmp(output[i].label, expected_output[i].label) != 0) {
			printf("Wrong label for index %d on test number %d\n", i, test_num); return false;
		}
		if (output[i].op_a != expected_output[i].op_a) {
			printf("Wrong op_a for index %d on test number %d\n", i, test_num); return false;
		}
		if (output[i].op_b != expected_output[i].op_b) {
			printf("Wrong op_b for index %d on test number %d\n", i, test_num); return false;
		}
		if (output[i].op_c != expected_output[i].op_c) {
			printf("Wrong op_c for index %d on test number %d\n", i, test_num); 
			printf("Expected %d, got %d\n", expected_output[i].op_c, output[i].op_c);
			return false;
		}
		if (output[i].address != expected_output[i].address) {
			printf("Wrong starting address for index %d on test number %d,\n", i, test_num);
			printf("Expected %d, got %d\n", expected_output[i].address, output[i].address); return false;
			
		}
	}
	
	return true;
}

// need this because the macro for pretty test colors wont take arguments for functions
static bool parser() {
	return 
		parser_passOne_test(1, "./asm/two_label.cade", expected_firstPass_1, 9) &&
		parser_passTwo_test(2, "./asm/two_label.cade", expected_second_pass_1, 3);
}


void run_parser_tests() {
	TEST_CHECK(parser);
}
