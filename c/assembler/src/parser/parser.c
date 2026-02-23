#include "parser.h"
#include "../util/dyn_array.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


Parser parser_init(Token *input) {
	Parser parser = {
		.input = input,
		.current_tok = input[0],
		.read_position = 0,
		.location_counter = 0,
		.current_instruction = 0
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

int32_t *parser_pass_two(Parser *self, Token *tokens_from_pass_one) {
	int32_t *output = DYN_ARRAY(int32_t);
	int i = 0;

	while (tokens_from_pass_one[i].type != Eof) {
		Token current = tokens_from_pass_one[i];

		if (current.type == SYMBOL) {
            // Special handling for 'N' (Next Instruction)
            if (strcmp(current.literal, "N") == 0) {
                int current_offset = ARRAY_LENGTH(output);
                int next_instr = current_offset + (3 - (current_offset % 3));
                ARRAY_PUSH(output, next_instr);
            } else {
			    ST_Entry *temp = HASH_FIND(self->symbol_table, current.literal, ST_Entry);
			    if (temp == NULL) {
				    fprintf(stderr, "Error: Undefined label '%s'\n", current.literal);
				    exit(1);
			    }
			    int32_t val = temp->address; 
			    ARRAY_PUSH(output, val);
            }
		} 
		else if (current.type == INT) {
			int32_t val = atoi(current.literal);
			ARRAY_PUSH(output, val);
		}

		i++;
	}
	return output;
}

