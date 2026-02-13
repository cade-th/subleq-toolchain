#include <stdio.h>
#include "lexer/lexer_tests.h"
#include "parser/parser_tests.h"
#include "encoder/encoder_tests.h"
#include "full/full_tests.h"

int main() {
	run_lexer_tests();
	run_parser_tests();
	run_encoder_tests();
	run_full_tests();
	return 0;
}
