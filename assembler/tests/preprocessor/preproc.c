#include "preproc.h"
#include "../../src/preprocessor/preproc.h"
#include "../../src/util/hashmap.h"
#include "../../src/util/io.h" 

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


static bool preproc_test_direct(int test_num, char *input) {

	File file = io_file_read("../../asm/one_macro.cade");

	Lexer lexer = lexer_init(input); Token *tokens = lex(&lexer);

	Preproc preproc = preproc_init(tokens);

	preprocess(&preproc);

	return false;
}

// This test is hardocded shit need to make a better one
static bool preproc_test_file(int test_num, char *path) {

	File file = io_file_read(path);

	Lexer lexer = lexer_init(file.data); Token *tokens = lex(&lexer);

	Preproc preproc = preproc_init(tokens);

	preprocess(&preproc);

	MNT_Entry *expected_1 = NULL;
	MNT_Entry *expected_2 = NULL;
	HASH_FIND(preproc.mnt, "VAR_1", expected_1, MNT_Entry);
	HASH_FIND(preproc.mnt, "VAR_2", expected_2, MNT_Entry);

	// Check if the correct insertions to the MNT are done
	if (strcmp(expected_1->key, "VAR_1") != 0) {
		printf("Wrong key for VAR_1\n");
		return false; 
	}
	if (strcmp(expected_2->key, "VAR_2") != 0) {
		printf("Wrong key for VAR_2\n");
		return false; 
	}

	// check the contents of the macros
	char *expected_literal_1 = "56";
	char *expected_literal_2 = "34";
	char *expected_literal_3 = "57";
	char *expected_literal_4 = "324";
	char *expected_literal_5 = "5";
	if (strcmp(preproc.mdt[0][0].literal, expected_literal_1) != 0
	||  strcmp(preproc.mdt[0][1].literal, expected_literal_2) != 0
	) {
		printf("Wrong Token Inserted into first macro\n");
		return false;
	}
	if (strcmp(preproc.mdt[1][0].literal, expected_literal_3) != 0
	||  strcmp(preproc.mdt[1][1].literal, expected_literal_4) != 0
	||  strcmp(preproc.mdt[1][2].literal, expected_literal_5) != 0
	) {
		printf("Wrong Token Inserted into second macro\n");
		return false;
	}

	return true;
}

static bool preproc() {
	return 
	 preproc_test_file(1, "asm/two_macro.cade");
}

void run_preproc_tests() {
	TEST_CHECK(preproc);
}
