#include "lexer.h"
#include <string.h>
#include "../util/dyn_array.h"
#include <ctype.h>
#include <stdbool.h>

void read_char(Lexer *self);

Lexer lexer_init(char *input) {
	Lexer lexer = {
		.input = input,
		.read_position = 0,
		.position = 0,
		.ch = 0
	};
	read_char(&lexer);
	return lexer;
}

Token *lex(Lexer *self) {
	Token *tokens = DYN_ARRAY(Token);

	while (true) {
		Token current_token = next_token(self);
		ARRAY_PUSH(tokens, current_token);
		if (current_token.type == Eof) break;
	}

	return tokens;
}

void read_char(Lexer *self) {
	// end condition for lexer
	if (self->read_position >= (int)strlen(self->input)) {
		self->ch = 0;
	}
	else {
		// aquire the next character for examination
		self->ch = self->input[self->read_position];
	}

	self->position = self->read_position;
	self->read_position += 1;
}

static void remove_comment(Lexer *self) {
	while (self->ch != '\n' && self->ch != 0) {
		read_char(self);
	}
}

static char *read_number(Lexer *self) {
	int position = self->position;
	while (isdigit(self->ch)) {
		read_char(self);
	}
	return strndup(self->input + position, self->position - position);
}

static char *read_symbol(Lexer *self) {
	int position = self->position;
    
	// Allow '.' at the start for local labels
	if (self->ch == '.') {
		read_char(self);
	}

	while (isalnum(self->ch) || self->ch == '_') {
		read_char(self);
	}
	return strndup(self->input + position, self->position - position);
}

Token next_token(Lexer *self) {
	Token tok = {
		.literal = " ",
		.type = Eof
	};

	skip_whitespace(self);

	switch (self->ch) {
		case '#': tok.type = HASH; tok.literal = strdup("#"); break;
		case '$': tok.type = DOLLAR; tok.literal = strdup("$"); break;
		case '(': tok.type = RPAREN; tok.literal = strdup("("); break;
		case ')': tok.type = LPAREN; tok.literal = strdup(")"); break;
		case ';': {
			remove_comment(self);
			return next_token(self);
		}
        case ',': {
            tok.type = COMMA;
            tok.literal = strdup(",");
            break;
        }
		case '-': {
			// Check if we have a negative number
			if (isdigit(self->input[self->read_position])) {
				int start_pos = self->position;
				read_char(self); // consume '-'
				
				while (isdigit(self->ch)) {
					read_char(self);
				}
				
				tok.type = INT;
				tok.literal = strndup(self->input + start_pos, self->position - start_pos);
				return tok;
			}
			// Otherwise fall through to default illegal handling or separate minus token
			tok.type = ILLEGAL;
			tok.literal = strndup("-", 1);
			read_char(self);
			return tok;
		}
		case 0: 
		    tok.type = Eof; 
		    tok.literal = strdup(" "); 
		    return tok; // Return immediately!
		default:
			if (isdigit(self->ch)) {
				tok.type = INT;
				tok.literal = read_number(self);
				return tok;
			}

			else if (isalpha(self->ch) || self->ch == '.' || self->ch == '_') {
				char *literal = read_symbol(self);
		    
				// PEEK: Is the current character a colon?
				if (self->ch == ':') {
					tok.type = LABEL; 
					tok.literal = literal;
					read_char(self); // Consume the ':' so it's not processed again
				} else {
					tok.type = SYMBOL;
					tok.literal = literal;
				}
				return tok;
			}
				
			else {
				tok.type = ILLEGAL;
				tok.literal = strndup(&self->ch, 1);
				read_char(self);
				return tok;
			}
	}

	read_char(self);
	return tok;
}

void skip_whitespace(Lexer *self) {
    // Use isspace() from <ctype.h> to catch all types of whitespace
    while (isspace(self->ch) || self->ch == (char)0xA0) { 
        read_char(self);
    }
}

