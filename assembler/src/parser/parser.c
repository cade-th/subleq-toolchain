#include "parser.h"
#include "../util/dyn_array.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


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
			};
			new.address = self->location_counter;
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

ParsedInstruction *parser_pass_two(Parser *self, Token *tokens_from_pass_one) {
	ParsedInstruction *output = DYN_ARRAY(ParsedInstruction);
	int i = 0;
	int counter = 0;

	// Reset instruction state
	self->current_instruction.address = 0; // Match your test expectation

	while (tokens_from_pass_one[i].type != Eof) {
		Token current = tokens_from_pass_one[i];

		if (current.type == SYMBOL) {
			ST_Entry *temp = HASH_FIND(self->symbol_table, current.literal, ST_Entry);
			int32_t val = temp ? temp->address : 0; 
	    
			if (counter == 0) self->current_instruction.op_a = val;
			else if (counter == 1) self->current_instruction.op_b = val;
			else if (counter == 2) self->current_instruction.op_c = val;
		} 
		else if (current.type == INT) {
			int32_t val = atoi(current.literal);
			if (counter == 0) self->current_instruction.op_a = val;
			else if (counter == 1) self->current_instruction.op_b = val;
			else if (counter == 2) self->current_instruction.op_c = val;
		}

		counter++;

		// If we have a full SUBLEQ instruction (3 operands)
		if (counter == 3) {
			ARRAY_PUSH(output, self->current_instruction);
			counter = 0;
			self->current_instruction.address += 3; // Standard SUBLEQ increment
		}
		i++	;
	}
	return output;
}

