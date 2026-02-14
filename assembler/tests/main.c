#include "lexer/lexer_tests.h"
#include "parser/parser_tests.h"
#include "encoder/encoder_tests.h"
#include "full/full_tests.h"
#include "preprocessor/preproc.h"

int main() {
	run_lexer_tests();
	run_parser_tests();
	run_encoder_tests();
	run_full_tests();
	run_preproc_tests();
	return 0;
}
