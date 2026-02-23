#include <stdbool.h>
#include <string.h>
#include "../..//src/lexer/lexer.h"
#include "lexer_tests.h"
#include "../../src/util/io.h"

char *input_2 = "        \
			 \
#define VAR_1 56	 \
endM                     \
			 \
START:                   \
	10 10 3          \
	10 12 3          \
";

Token expected_2[] = {
	{"#", HASH},
	{"define", SYMBOL},
	{"VAR_1", SYMBOL},
	{"56", INT},	
	{"endM", SYMBOL},
	{"START", LABEL},
	{"10", INT},
	{"10", INT},
	{"3", INT},
	{"10", INT},
	{"12", INT},
	{"3", INT},
	{" ", Eof}
};

Token expected_3[] = {
	{"START", LABEL},
	{"34",    INT}, 
	{"5",     INT},
	{"3",     INT},
	{"23",    INT},
	{"5",     INT},
	{"LOOP",  SYMBOL},
	{"LOOP",  LABEL},
	{"45",    INT},   
	{"3",     INT},  
	{"6",     INT}, 
	{" ",     Eof} 
};


static Token expected_4[] = {
	{"#", HASH},
	{"define", SYMBOL },
	{"JMP", SYMBOL},
	{"(", RPAREN},
	{"dest", SYMBOL},
	{")", LPAREN},
	{"0", INT},
	{"0", INT},
	{"dest", SYMBOL},
	{"endM", SYMBOL},
	{"START", LABEL},
	{"45", INT},
	{"56", INT},
	{"6", INT},
	{"JMP", SYMBOL},
	{"(", RPAREN},
	{"2", INT},
	{")", LPAREN},
	{" ", Eof},
};

static Token expected_5[] = {

    	{"#", HASH},
	{"define", SYMBOL },
	{"TARGET", SYMBOL},
	{"(", RPAREN},
	{"T", SYMBOL},
	{")", LPAREN},
	{"T", SYMBOL},
	{"endM", SYMBOL},
	{"#", HASH},
	{"define", SYMBOL },
	{"JMP", SYMBOL},
	{"(", RPAREN},
	{"dest", SYMBOL},
	{")", LPAREN},
	{"0", INT},
	{"0", INT},
	{"TARGET", SYMBOL},
        {"(", RPAREN},
	{"dest", SYMBOL},
	{")", LPAREN},
	{"endM", SYMBOL},
	{"START", LABEL},
	{"34", INT},
	{"4", INT},
	{"5", INT},
	{"JMP", SYMBOL},
	{"(", RPAREN},
	{"2", INT},
	{")", LPAREN},
	{" ", Eof},
};

static Token expected_negative[] = {
	{"-5", INT},
	{"-12", INT},
	{"0", INT},
	{" ", Eof}
};


char* token_type_to_str(Token_t type) {
    switch(type) {
        case SYMBOL: return "SYMBOL";
        case LABEL:  return "LABEL";
        case INT:    return "INT";
        default:     return "UNKNOWN";
    }
}

bool compare_tokens(int index, Token expected, Token actual) {
    if (actual.type != expected.type) {
        printf("\n[FAILED] Token Type Mismatch at index %d\n", index);
        printf("  Expected: %-10s (type %d)\n", token_type_to_str(expected.type), expected.type);
        printf("  Got:      %-10s (type %d)\n", token_type_to_str(actual.type), actual.type);
        printf("  Literal context: \"%s\"\n", actual.literal);
        return false;
    }

    if (strcmp(actual.literal, expected.literal) != 0) {
        printf("\n[FAILED] Token Literal Mismatch at index %d\n", index);
        printf("  Expected: \"%s\"\n", expected.literal);
        printf("  Got:      \"%s\"\n", actual.literal);
        printf("  Type:     %s\n", token_type_to_str(actual.type));
        return false;
    }

    return true;
}

static bool lexer_basic_test(char *input, int expected_size, Token expected_tokens[]) {
    Lexer lexer = lexer_init(input);
    Token *output = lex(&lexer);

    for (int i = 0; i < expected_size; i++) {
        if (!compare_tokens(i, expected_tokens[i], output[i])) {
            return false;
        }
    }
    
    return true;
}

static bool lexer_file_test(char *file_path, int expected_size, Token expected_tokens[]) {
    File file = io_file_read(file_path);

    Lexer lexer = lexer_init(file.data);
    Token *output = lex(&lexer);

    for (int i = 0; i < expected_size; i++) {
        if (!compare_tokens(i, expected_tokens[i], output[i])) {
            return false;
        }
    }

    return true;
}

static bool lexer() {
	return 
	lexer_basic_test(input_2, 13, expected_2) &&
	lexer_file_test("./asm/two_label.cade", 12, expected_3) &&	
	lexer_file_test("./asm/macro_arguments.cade", 19, expected_4) &&
	lexer_file_test("./asm/recursive.cade", 30, expected_5) &&
	lexer_basic_test("-5 -12 0", 4, expected_negative);	
}


void run_lexer_tests() {
	TEST_CHECK(lexer);
}
