#include "parser.h"
#include "../util/dyn_array.h"
#include <stdio.h>
#include <string.h>


Parser parser_init(Token *input) {
	ParsedInstruction current = {
		.label = "subleq",
		.op_a = 0,
		.op_b = 0,
		.op_c = 0,
		.address = 0,
	};

	Parser parser = {
		.input = input,
		.current_tok = input[0],
		.read_position = 0,
		.current_instruction = current,
		.location_counter = 0
		
	};
	return parser;
}
static void consume(Parser *self) {
	self->current_tok = self->input[self->read_position];	
	self->read_position++;
}

static void parse_label(Parser *self) {
	if (strcmp(self->current_tok.literal, "START") == 0) {
		consume(self);
	}
	else {
		// make an entry into the symbol table
	}
}

Token *parser_pass_one(Parser *self) {

	Token *output = DYN_ARRAY(Token);

	int i = 0;
	while (self->input[i].type != Eof) {

		self->current_tok = self->input[i];
	
		if (self->current_tok.type == LABEL) {
			ST_Entry new = {
				.key = self->current_tok.literal,
				.address = self->location_counter
			};
			HASH_PUSH(self->symbol_table, new.key, new, ST_Entry);
		}
		else {
			ARRAY_PUSH(output, self->current_tok);
			self->location_counter++;
		}
		i++;
		
	}
	// Always append the EOF token so the next pass knows when to stop
	Token eof_tok = { .type = Eof, .literal = strdup(" ") };
	ARRAY_PUSH(output, eof_tok);

	return output;
}

static ParsedInstruction *parser_pass_two(Parser *self, Token *input) {
	int i = 0;
	while (self->input[i].type != Eof) {

		if (self->input[i].type == SYMBOL) {
					
		}
	}

}

// just trying to parse a single instruction for now
ParsedInstruction *parse(Parser *self) {

	ParsedInstruction *output = DYN_ARRAY(ParsedInstruction);

	consume(self);
	
	while (self->current_tok.type != Eof) {
		// handle subroutines
		// has to be some sort of label here	
		if (self->current_tok.type == LABEL) {
			parse_label(self);	
		}
		self->current_instruction.op_a = self->current_tok.literal;
		consume(self);

		if (self->current_tok.type == Eof) break; // Safety check
		self->current_instruction.op_b = self->current_tok.literal;
		consume(self);

		if (self->current_tok.type == Eof) break; // Safety check
		self->current_instruction.op_c = self->current_tok.literal;
		consume(self);

		ARRAY_PUSH(output, self->current_instruction);
}

	return output;
	
}
