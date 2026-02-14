#include "preproc.h"
#include "../../src/preprocessor/preproc.h"

// finding symbols should be easy just by looking at next and seeing if it's a colon
static char *input_1 = "        \
			 \
#define VAR 7            \
\
#define VAR_2 3          \
			 \
#ifdef		         \
			 \
START:                   \
	10 10 3          \
	10 12 3          \
";

static bool preproc_test(int test_num, char *input) {

	Lexer lexer = lexer_init(input); Token *tokens = lex(&lexer);

	Preproc preproc = preproc_init(tokens);

	preprocess(&preproc);

	return false;
}

static bool preproc() {
	return preproc_test(1, input_1);
}

void run_preproc_tests() {
	TEST_CHECK(preproc);
}
