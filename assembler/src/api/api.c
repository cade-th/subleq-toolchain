#include "api.h"
#include <stdio.h>

// Project API structure/wishlist
// Allows for ease of use and incremental steps in the assembling process

/*
Token *tokenize_api(char *src_code) {
	Lexer lexer = lexer_init(src_code);Token *output = lex(&lexer);
	return output;
}

ParsedInstruction *parse_api(char *src_code) {
	Lexer lexer = lexer_init(src_code);Token *tokens = lex(&lexer);
	Parser parser = parser_init(tokens); ParsedInstruction *output = parse(&parser);
	return output;
}

uint8_t *encode_api(char *src_code) {
	Lexer lexer = lexer_init(src_code);Token *tokens = lex(&lexer);
	Parser parser = parser_init(tokens); ParsedInstruction *instructions = parse(&parser);
	// need to fix this by omitting the count of bytes somehow
	Encoder encoder = encoder_init(instructions); uint8_t *output = encode(&encoder, 3);
	return output;
}

void assemble_api(char *src_code) {
	Lexer lexer = lexer_init(src_code);Token *tokens = lex(&lexer);
	Parser parser = parser_init(tokens); ParsedInstruction *instructions = parse(&parser);
	// need to fix this by omitting the count of bytes somehow
	Encoder encoder = encoder_init(instructions); uint8_t *output = encode(&encoder, 3);
	FILE *file = fopen("output.bin", "wb");
	size_t code_size = sizeof(output);
	fwrite(output, 1, code_size, file);
	fclose(file);
}
*/
