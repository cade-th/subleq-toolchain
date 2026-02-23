#include "parser_tests.h"
#include "../../src/parser/parser.h"
#include "../lexer/lexer_tests.h"
#include <stdbool.h>
#include "../../src/util/io.h"
#include <string.h>




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

static int32_t expected_second_pass_1[] = {
	34, 5, 3,
	23, 5, 6,
	45, 3, 6
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
	// Note addresses are now indices in flat array (so *3 compared to instruction count)
    // Wait, location_counter in pass_one?
    // In pass_one: "self->location_counter++;" happens for every token in "else" block.
    // So location_counter counts WORDS (ints), which is correct for Subleq.
    
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
	int32_t expected_output[], 
	int expected_size
	) {
	File file = io_file_read(path);

	Lexer lexer = lexer_init(file.data); Token *tokens = lex(&lexer);
	Parser parser = parser_init(tokens);

	Token *one = parser_pass_one(&parser);
	int32_t *output = parser_pass_two(&parser, one);

	for (int i=0; i < expected_size; i++) {
		if (output[i] != expected_output[i]) {
            printf("Mismatch at index %d: expected %d, got %d\n", i, expected_output[i], output[i]);
            return false;
        }
	}
	
	return true;
}

// need this because the macro for pretty test colors wont take arguments for functions
static bool parser() {
	return 
		parser_passOne_test(1, "./asm/two_label.cade", expected_firstPass_1, 9) &&
		parser_passTwo_test(2, "./asm/two_label.cade", expected_second_pass_1, 9);
}


void run_parser_tests() {
	TEST_CHECK(parser);
}
