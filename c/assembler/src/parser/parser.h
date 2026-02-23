#pragma once
#include "../lexer/lexer.h"
#include <stdint.h>
#include "../util/hashmap.h"

// The parser outputs this IR that then gets encoded by the encoder
// The parser outputs this IR that then gets encoded by the encoder

typedef struct {
	union {
		uint8_t data_address;
		uint16_t code_address;
	};
} Symbol;

typedef struct {
	char *key;
	uint32_t address;
} ST_Entry;

typedef struct {
	Token *input;
	Token current_tok;	
	int read_position;
	int32_t current_instruction;
	uint16_t location_counter;
	ST_Entry *symbol_table[HASH_CAPACITY];
} Parser;

Parser parser_init(Token *input);
int32_t *parse(Parser *self);
Token *parser_pass_one(Parser *self);
int32_t *parser_pass_two(Parser *self, Token *input);
