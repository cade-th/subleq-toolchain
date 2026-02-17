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
		.read_position = 0,
		.position = 0,
		.macro_count = 0,
	};
	return preproc;
}

static Token peek(int count, Preproc *self) {
	return self->input[self->position + count];	
}

static void parse_macro_header(Preproc *self, MNT_Entry *entry) {
	// skip the hash and define
	self->read_position+=2;	
	// get the name
	entry->key = strdup(self->input[self->read_position].literal);
	self->read_position++;

	//Handle Parameters 
	if (self->input[self->read_position].type == RPAREN) {
		self->read_position++; // Move past '('
		int para_count = 0;

		while (self->input[self->read_position].type != LPAREN && 
			self->input[self->read_position].type != Eof) 
		{
    
			if (self->input[self->read_position].type == SYMBOL) {
				Parameter p = { .position = para_count++, .value = self->input[self->read_position] };
				ARRAY_PUSH(entry->parameters, p);
			}
			// skip commas	
			// if (self->input[self->read_position].type == COMMA) self->read_position++;
			self->read_position++;
		}

		if (self->input[self->read_position].type == LPAREN) {
			self->read_position++; // Move past ')'
		}
	}
}

static void parse_macro_body(Preproc *self) {
    while (self->input[self->read_position].type != Eof) {
        Token t = self->input[self->read_position];
        
        // Stop if we hit the endM keyword
        if (t.literal && strcmp(t.literal, "endM") == 0) {
            self->read_position++; 
            break;
        }

        ARRAY_PUSH(self->mdt[self->macro_count], t);
        self->read_position++;
    }
}

static void parse_macro(Preproc *self) {
    // 1. Initialize the entry and MDT storage
    MNT_Entry new = {
        .mdt_index = self->macro_count,
        .parameters = DYN_ARRAY(Parameter)
    };
    self->mdt[self->macro_count] = DYN_ARRAY(Token);

    self->read_position = self->position; 
    parse_macro_header(self, &new);

    parse_macro_body(self);

    HASH_PUSH(self->mnt, new.key, new, MNT_Entry);
    self->position = self->read_position - 1; 
    self->macro_count++;
}


void pass_one(Preproc *self) {

	// loop through the tokens to find #define
	#define CURRENT self->input[self->position]

	// first pass
	while (CURRENT.type != Eof) {
		if (CURRENT.type == HASH) {
			if (strcmp(peek(1, self).literal, "define") == 0){
				parse_macro(self);
			}
		}
		self->position++;
	}
}

static int find_param_index(MNT_Entry *macro, char *literal) {
    if (literal == NULL) return -1;
    for (int i = 0; i < ARRAY_LENGTH(macro->parameters); i++) {
        if (strcmp(macro->parameters[i].value.literal, literal) == 0) {
            return i;
        }
    }
    return -1;
}

static Token* collect_arguments_from_slice(Token *tokens, int length, int *pos) {
    Token *args = DYN_ARRAY(Token);

    // 1. Boundary check: Is there even a next token?
    // 2. Type check: Is it the opening paren '(' (your RPAREN)?
    if (*pos >= length || tokens[*pos].type != RPAREN) {
        return args;
    }

    (*pos)++; // Move past '('

    // Loop until we find the closing paren ')' (your LPAREN) or hit the end
    while (*pos < length && tokens[*pos].type != LPAREN) {
        
        // In a more complex preprocessor, you'd handle nested parens here.
        // For now, we just capture everything that isn't a comma.
        if (tokens[*pos].type != COMMA) {
            // It's important to push a copy or the original token
            ARRAY_PUSH(args, tokens[*pos]);
        }
        
        (*pos)++;
        
        // Safety break to prevent infinite loops on malformed code
        if (*pos >= length) break;
    }

    // Move past the closing ')' if we found it
    if (*pos < length && tokens[*pos].type == LPAREN) {
        (*pos)++;
    }

    return args;
}

// 1. Updated substitution logic to prevent invalid memory access
static void expand_tokens(Preproc *self, Token *input, int length, Token **output) {
    int i = 0;
    while (i < length) {
        Token current = input[i];

        if (current.type == SYMBOL) {
            MNT_Entry *macro = HASH_FIND(self->mnt, current.literal, MNT_Entry);
            if (macro != NULL) {
                int next_pos = i + 1;
                Token *actual_args = collect_arguments_from_slice(input, length, &next_pos);
                int arg_count = ARRAY_LENGTH(actual_args);

                // Create the substituted body
                Token *substituted_body = DYN_ARRAY(Token);
                Token *body = self->mdt[macro->mdt_index];

                for (int b = 0; b < ARRAY_LENGTH(body); b++) {
                    int param_idx = find_param_index(macro, body[b].literal);
                    if (param_idx != -1 && param_idx < arg_count) {
                        ARRAY_PUSH(substituted_body, actual_args[param_idx]);
                    } else {
                        ARRAY_PUSH(substituted_body, body[b]);
                    }
                }

                // RECURSE on the substituted body
                expand_tokens(self, substituted_body, ARRAY_LENGTH(substituted_body), output);

                // IMPORTANT: In a real C project, you must free substituted_body here 
                // to avoid the memory leak that likely caused your crash.
                // free(substituted_body); 

                i = next_pos; 
                continue;
            }
        }

        // Push non-macro tokens to final output
        ARRAY_PUSH(*output, current);
        i++;
    }
}

// 2. THE ENTRY POINT: Filter and Start
Token *pass_two(Preproc *self) {
    Token *final_output = DYN_ARRAY(Token);
    Token *clean_input = DYN_ARRAY(Token);

    // Filter out the #define ... endM blocks from the original input
    int i = 0;
    while (self->input[i].type != Eof) {
        if (self->input[i].type == HASH) {
            while (self->input[i].type != Eof && 
                  (self->input[i].literal == NULL || strcmp(self->input[i].literal, "endM") != 0)) {
                i++;
            }
            if (self->input[i].type != Eof) i++;
            continue;
        }
        ARRAY_PUSH(clean_input, self->input[i]);
        i++;
    }

    // Kick off the recursion
    expand_tokens(self, clean_input, ARRAY_LENGTH(clean_input), &final_output);

    Token eof_tok = { .type = Eof, .literal = " " };
    ARRAY_PUSH(final_output, eof_tok);
    return final_output;
}



Token *preprocess(Preproc *self) {
	pass_one(self);	
	Token * output = pass_two(self);
	return output;
}




