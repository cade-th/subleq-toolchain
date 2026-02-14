#include "parser.h"
#include "../util/dyn_array.h"
#include <stdio.h>


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
		.position = input[0],
		.start_position = 0,
		.current_instruction = current

	};
	return parser;
}
static void consume(Parser *self) {
	self->current_tok = self->input[self->read_position];	
	self->read_position++;
}

// just trying to parse a single instruction for now
ParsedInstruction *parse(Parser *self) {

	ParsedInstruction *output = DYN_ARRAY(ParsedInstruction);

	// this segfaults
	consume(self);
	
	while (self->current_tok.type != Eof) {
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
