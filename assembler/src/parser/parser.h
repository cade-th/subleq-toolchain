#pragma once
#include "../lexer/lexer.h"
#include <stdint.h>

typedef struct {
	Token *tokens;
} Macro;

typedef struct {
	Token *label;
	union{
		uint16_t address;
		uint8_t value;
	};
}Label ;

// The parser outputs an array of these that then gets encoded by the encoder
typedef struct {
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
	// these will both be dynamic arrays
	// Hash table for Macro name table (MNT)
	// dynamic array for Macro Definition Table (MDT)
	Macro *MDT;
	Label *symbol_table;
} Parser;

Parser parser_init(Token *input);
void parse_macro(Parser *self);
void parse_label(Parser *self);
ParsedInstruction *parse(Parser *self);
