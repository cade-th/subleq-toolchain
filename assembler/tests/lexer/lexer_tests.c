#include <stdbool.h>
#include <string.h>
#include "../..//src/lexer/lexer.h"
#include "lexer_tests.h"

char *input_2 = "        \
			 \
#define VAR 7            \ 
			 \
START:                   \
	10 10 3          \
	10 12 3          \
";

Token expected_2[] = {
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

bool lexer_basic_test(char *input, int expected_tokens_size, Token expected_tokens[]) {
	

	Lexer lexer = lexer_init(input);

	Token *output = lex(&lexer);
	// need to check size before this
	for (int i=0; i < expected_tokens_size; i++) {
		if (output[i].type != expected_tokens[i].type) {
			printf("Incorrect token type for index %d: expected %d, got %d\n", i, expected_tokens[i].type, output[i].type);
			return false;	
		};
		if (strcmp(output[i].literal, expected_tokens[i].literal)) {
			printf("Incorrect token literal for index %d: expected %s, got %s\n", i, expected_tokens[i].literal, output[i].literal);	
			return false;
		}
	};
	
	return true;
}

bool lexer() {
	return lexer_basic_test(input_2, 13, expected_2);	
}


void run_lexer_tests() {
	TEST_CHECK(lexer);
}
