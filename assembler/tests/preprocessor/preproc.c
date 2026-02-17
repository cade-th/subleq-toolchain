#include "preproc.h"
#include "../../src/preprocessor/preproc.h"
#include "../../src/util/hashmap.h"
#include "../../src/util/io.h" 

// This test is hardocded shit need to make a better one
static bool preproc_test_passOne(int test_num, char *path) {

	File file = io_file_read(path);

	Lexer lexer = lexer_init(file.data); Token *tokens = lex(&lexer);

	Preproc preproc = preproc_init(tokens);

	pass_one(&preproc);

	MNT_Entry *expected_1 = HASH_FIND(preproc.mnt, "VAR_1", MNT_Entry);
	MNT_Entry *expected_2 = HASH_FIND(preproc.mnt, "VAR_2", MNT_Entry);

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
	char *expected_literal_3 = "32";
	char *expected_literal_4 = "57";
	char *expected_literal_5 = "324";
	char *expected_literal_6 = "5";
	if (strcmp(preproc.mdt[0][0].literal, expected_literal_1) != 0
	||  strcmp(preproc.mdt[0][1].literal, expected_literal_2) != 0
	||  strcmp(preproc.mdt[0][2].literal, expected_literal_3) != 0
	) {
		printf("Wrong Token Inserted into first macro\n");
		return false;
	}
	if (strcmp(preproc.mdt[1][0].literal, expected_literal_4) != 0
	||  strcmp(preproc.mdt[1][1].literal, expected_literal_5) != 0
	||  strcmp(preproc.mdt[1][2].literal, expected_literal_6) != 0
	) {
		printf("Wrong Token Inserted into second macro\n");
		return false;
	}

	return true;
}

Token expected_output_1[] = {
	{"START", LABEL},
	{"56", INT},
	{"34", INT},
	{"32", INT},
	{"57", INT},
	{"324", INT},
	{"5", INT},
	{"3", INT},
	{"9", INT},
	{"3", INT},
	{"2", INT},
	{"3", INT},
	{"4", INT}
};



static bool preproc_test_arguments_1(int test_num, char *path) {

	File file = io_file_read(path);

	Lexer lexer = lexer_init(file.data); Token *tokens = lex(&lexer);

	Preproc preproc = preproc_init(tokens);

	pass_one(&preproc);

	MNT_Entry *expected_1 = HASH_FIND(preproc.mnt, "JMP", MNT_Entry);


	// Check if the correct insertions to the MNT are done
	if (strcmp(expected_1->key, "JMP") != 0) {
		printf("Wrong key for JMP\n");
		return false; 
	}

	if (ARRAY_LENGTH(expected_1->parameters) != 1) {
		printf("Incorrect number of arguments, got %d\n", ARRAY_LENGTH(expected_1->parameters));
		return false;
	}

	// check the contents of the macro
	if (strcmp(expected_1->parameters[0].value.literal, "dest") != 0) {
		printf("Paramter value wrong\n");
		return false;
	}

	return true;
}

static Token expected_output_arguments[] = {
	{"START", LABEL},
	{"45", INT},
	{"56", INT},
	{"6", INT},
	{"0", INT},
	{"0", INT},
	{"2", INT},
	{" ", Eof}
};

static Token expected_recursion[] = {
    {"START", LABEL},
    {"34", INT},
    {"4", INT},
    {"5", INT},
    {"0", INT},     // From JMP
    {"0", INT},     // From JMP
    {"2", INT},     // Expanded from TARGET(2)
    {" ", Eof},
};

static bool preproc_test_recursive_1(int test_num, char *path) {

	File file = io_file_read(path);

	Lexer lexer = lexer_init(file.data); Token *tokens = lex(&lexer);

	Preproc preproc = preproc_init(tokens);

	pass_one(&preproc);

	MNT_Entry *expected_1 = HASH_FIND(preproc.mnt, "TARGET", MNT_Entry);
	MNT_Entry *expected_2 = HASH_FIND(preproc.mnt, "JMP", MNT_Entry);


	// Check if the correct insertions to the MNT are done
	if (strcmp(expected_2->key, "JMP") != 0) {
		printf("Wrong key for JMP\n");
		return false; 
	}

	// Check if the correct insertions to the MNT are done
	if (strcmp(expected_1->key, "TARGET") != 0) {
		printf("Wrong key for JMP\n");
		return false; 
	}


	if (ARRAY_LENGTH(expected_2->parameters) != 1) {
		printf("Incorrect number of arguments, got %d\n", ARRAY_LENGTH(expected_2->parameters));
		return false;
	}

	if (ARRAY_LENGTH(expected_1->parameters) != 1) {
		printf("Incorrect number of arguments, got %d\n", ARRAY_LENGTH(expected_1->parameters));
		return false;
	}

	// check the contents of the macro
	if (strcmp(expected_2->parameters[0].value.literal, "dest") != 0) {
		printf("Parameter for JMP value wrong\n");
		return false;
	}
	
	if (strcmp(expected_1->parameters[0].value.literal, "T") != 0) {
		printf("Parameter value for TARGET wrong\n");
		return false;
	}

	return true;
}


// testing pass 2 for arguments
static bool preproc_test_recursive_2(
	int test_num, 
	char *path, 
	Token *expected_tokens, 
	int expected_tokens_size) 
{

	File file = io_file_read(path);

	Lexer lexer = lexer_init(file.data); Token *tokens = lex(&lexer);

	Preproc preproc = preproc_init(tokens);

	Token *output = preprocess(&preproc);

	for (int i=0; i < expected_tokens_size; i++) {
		if (output[i].type != expected_tokens[i].type) {
			printf("Test #%d: Incorrect token type for index %d: expected %d, got %d\n", test_num ,i, expected_tokens[i].type, output[i].type);
			return false;	
		};
		if (strcmp(output[i].literal, expected_tokens[i].literal)) {
			printf("Test #%d: Incorrect token literal for index %d: expected %s, got %s\n", test_num, i, expected_tokens[i].literal, output[i].literal);	
			return false;
		}
	};

	return true;
}




// testing pass 2 for arguments
static bool preproc_test_arguments_2(
	int test_num, 
	char *path, 
	Token *expected_tokens, 
	int expected_tokens_size) 
{

	File file = io_file_read(path);

	Lexer lexer = lexer_init(file.data); Token *tokens = lex(&lexer);

	Preproc preproc = preproc_init(tokens);

	Token *output = preprocess(&preproc);

	for (int i=0; i < expected_tokens_size; i++) {
		if (output[i].type != expected_tokens[i].type) {
			printf("Test #%d: Incorrect token type for index %d: expected %d, got %d\n", test_num ,i, expected_tokens[i].type, output[i].type);
			return false;	
		};
		if (strcmp(output[i].literal, expected_tokens[i].literal)) {
			printf("Test #%d: Incorrect token literal for index %d: expected %s, got %s\n", test_num, i, expected_tokens[i].literal, output[i].literal);	
			return false;
		}
	};

	return true;
}

static bool preproc_test_passTwo(int test_num, char *path, Token *expected_tokens, int expected_tokens_size) {

	File file = io_file_read(path);

	Lexer lexer = lexer_init(file.data); Token *tokens = lex(&lexer);

	Preproc preproc = preproc_init(tokens);

	Token *output = preprocess(&preproc);

	// need to check size before this
	for (int i=0; i < expected_tokens_size; i++) {
		if (output[i].type != expected_tokens[i].type) {
			printf("Test #%d: Incorrect token type for index %d: expected %d, got %d\n", test_num, i, expected_tokens[i].type, output[i].type);
			return false;	
		};
		if (strcmp(output[i].literal, expected_tokens[i].literal)) {
			printf("Test #%d: Incorrect token literal for index %d: expected %s, got %s\n",test_num, i, expected_tokens[i].literal, output[i].literal);	
			return false;
		}
	};


	return true;
}


static bool preproc() {
	return 
	 preproc_test_passOne(1, "asm/two_macro.cade") &&
	 preproc_test_passTwo(2, "asm/two_macro.cade", expected_output_1, 13) &&
	 preproc_test_arguments_1(3, "asm/macro_arguments.cade") &&
	 preproc_test_arguments_2(4, "asm/macro_arguments.cade", expected_output_arguments, 8) &&
	 preproc_test_recursive_1(5, "asm/recursive.cade") &&
	 preproc_test_recursive_2(6, "asm/recursive.cade", expected_recursion, 8);
}

void run_preproc_tests() {
	TEST_CHECK(preproc);
}
