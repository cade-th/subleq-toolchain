#pragma once
#include "../lexer/lexer.h"
#include <stdint.h>
#include "../util/hashmap.h"

// The parser outputs this IR that then gets encoded by the encoder
typedef struct {
	// could also just call this IR
	// probbly just make these all subleq
	char *label;
	char *op_a;
	char *op_b;
	char *op_c;
	uint16_t address;
} ParsedInstruction;

typedef struct {
	union {
		uint8_t data_address;
		uint16_t code_address;
	};
} Symbol;

typedef struct {
	char *key;
	uint16_t address;
} ST_Entry;

typedef struct {
	Token *input;
	Token current_tok;	
	int read_position;
	ParsedInstruction current_instruction;
	uint16_t location_counter;
	ST_Entry *symbol_table[HASH_CAPACITY];
} Parser;

Parser parser_init(Token *input);
ParsedInstruction *parse(Parser *self);
Token *parser_pass_one(Parser *self);
