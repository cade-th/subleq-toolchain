#include "full_tests.h"

#include "../../src/lexer/lexer.h"
#include "../../src/parser/parser.h"
#include "../../src/encoder/encoder.h"
#include <string.h>
#include "../../src/util/dyn_array.h"

static char *src_code_1 = " \
	0 1 2      \
";


static uint8_t expected_1[] = {
	0, 1, 2
};

static char *src_code_2 = " \
	0 1 2               \
	3 4 5               \
";


static uint8_t expected_2[] = {
	0, 1, 2,
	3, 4, 5
};



static bool full_test(
	int test_num,
	char *src_code,
	uint8_t expected[],
	int num_bytes
) {
	Lexer lexer = lexer_init(src_code); Token *tokens = lex(&lexer);
	Parser parser = parser_init(tokens); ParsedInstruction *IR = parse(&parser);
	Encoder encoder = encoder_init(IR); uint8_t *output = encode(&encoder, ARRAY_LENGTH(IR));


	// compare shit
	if (memcmp(output, expected, num_bytes) != 0) {
		printf("Test #%d, some byte is wrong\n", test_num);	
		return false;
	}
	
	return true;

}

static bool full() {
	return full_test(1,src_code_1, expected_1, 3) && full_test(2,src_code_2, expected_2, 6);
}

void run_full_tests() {
	TEST_CHECK(full);
}
