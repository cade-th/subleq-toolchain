#pragma once
#include "../lexer/lexer.h"
#include <stdint.h>
// The parser outputs an array of these that then gets encoded by the encoder
typedef struct {
	// could also just call this IR
	// probbly just make these all subleq
	char *label;
	char *op_a;
	char *op_b;
	char *op_c;
	int address;
} ParsedInstruction;

typedef struct {
	Token *input;
	Token current_tok;	
	int read_position;
	Token position;
	uint16_t start_position;
	ParsedInstruction current_instruction;
} Parser;

Parser parser_init(Token *input);
ParsedInstruction *parse(Parser *self);

