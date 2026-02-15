#include "preproc.h"
#include <stdio.h>
#include <string.h>
#include "../util/hashmap.h"

Preproc preproc_init(Token *input) {
	Preproc preproc = {
		.input = input,
		// make this a hash map probably
		.mnt = { 0 },
		.mdt = { 0 },
		.macro_count = 0,
	};
	return preproc;
}

void pass_one(Preproc *self) {
	// loop through the tokens to find #define
	int temp = 0;
	#define CURRENT self->input[temp]
	#define NEXT self->input[temp + 1]
	#define NEXT_NEXT self->input[temp + 2]

	// first pass
	while (CURRENT.type != Eof) {
		if (CURRENT.type == HASH) {
			self->mdt[self->macro_count] = DYN_ARRAY(Token);
			// Changing this if we want macros to have dependencies via ifdef etc
			if (strcmp(NEXT.literal, "define") == 0){

				// record the macro in the MNT
				MNT_Entry new = {
					.key = NEXT_NEXT.literal,
					.mdt_index = self->macro_count
				};
				// put the inner pointer ahead of the macro identifer according to our syntax
				int temp2 = temp +=3;
				#define INNER_CURRENT self->input[temp2]
				while (strcmp(INNER_CURRENT.literal, "endM") != 0) {
					// now in this loop we can push tokens into shit
					ARRAY_PUSH(self->mdt[self->macro_count], self->input[temp2]);
					// printf("Token Added\n");
					temp2++;	
				}
				// Push the macro into the MNT
				HASH_PUSH(self->mnt, new.key, new, MNT_Entry);
				// printf("macro found!\n");
			}
			// we know we're in a macro now

		self->macro_count++;
		}
		temp++;
	}
}

Token *pass_two(Preproc *self) {
	int temp = 0;
	Token *input = self->input;
	Token *new_output = DYN_ARRAY(Token);

	while (input[temp].type != Eof) {
        
		if (input[temp].type == HASH) {
			// we found a macro
			// skip tokens until endM
			while (input[temp].type != Eof && strcmp(input[temp].literal, "endM") != 0) {
				temp++;
			}
			// Skip the endM token 
			if (input[temp].type != Eof) {
				temp++;
			}
			continue;
		}

		// Handle Macro Expansion
		if (input[temp].type == SYMBOL) {
			// look mom I made a hash map in c
			MNT_Entry *macro = HASH_FIND(self->mnt, input[temp].literal, MNT_Entry);

			if (macro != NULL) {
				// Macro found
				Token *macro_body = self->mdt[macro->mdt_index];
				int body_len = ARRAY_LENGTH(macro_body);

				// Push every token from the macro definition
				for (int i = 0; i < body_len; i++) {
					ARRAY_PUSH(new_output, macro_body[i]);
				}
			} else {
				// It's a regular symbol (like a Label or Instruction), just push it
				ARRAY_PUSH(new_output, input[temp]);
			}
		} 
		else {
			ARRAY_PUSH(new_output, input[temp]);
		}
		temp++;
	}

    // have to add this for the parser
    Token eof_tok = { .type = Eof, .literal = " " };
    ARRAY_PUSH(new_output, eof_tok);

    return new_output;
}

Token *preprocess(Preproc *self) {
	pass_one(self);	
	Token * output = pass_two(self);
	return output;
	

}




