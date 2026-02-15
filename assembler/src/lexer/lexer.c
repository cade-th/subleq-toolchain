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
    // Symbols usually allow letters, numbers (after the first char), and underscores
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
		case ':': tok.type = COLON; tok.literal = strdup(":"); break;
		case ';': {
			remove_comment(self);
			return next_token(self);
		}
		case 0: tok.type = Eof; tok.literal = strdup(" "); break;

		default:
			if (isdigit(self->ch)) {
				tok.type = INT;
				tok.literal = read_number(self);
				return tok;
			}
			else if (isalpha(self->ch)) {
				tok.type = SYMBOL;
				tok.literal = read_symbol(self);
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
	while (self->ch == ' ' || self->ch == '\t' || self->ch == '\n' || self->ch == '\r') {
		read_char(self);
	}
}
