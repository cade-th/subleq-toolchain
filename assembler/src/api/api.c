#include "api.h"
#include <stdio.h>
#include "../preprocessor/preproc.h"
#include "../util/dyn_array.h"
#include "../parser/parser.h"

// Project API structure/wishlist
// Allows for ease of use and incremental steps in the assembling process

void assemble_api(char *src_code) {
	Lexer lexer = lexer_init(src_code);
	Token *tokens = lex(&lexer);

	Preproc preproc = preproc_init(tokens);
	Token *preprocessed_tokens = preprocess(&preproc);

	Parser parser = parser_init(preprocessed_tokens);
	parser_pass_one(&parser);
	int32_t *instructions = parser_pass_two(&parser, preprocessed_tokens);
	
	// Assuming DYN_ARRAY stores length in header as per dyn_array.h
	int instruction_count = ARRAY_LENGTH(instructions);
	Encoder encoder = encoder_init(instructions);
	int32_t *output = encode(&encoder, instruction_count);
	
	FILE *file = fopen("output.bin", "wb");
	if (file) {
		fwrite(output, sizeof(int32_t), instruction_count, file);
		fclose(file);
	} else {
		perror("Failed to open output.bin");
	}
}
