#pragma once

typedef enum {
	MNEMONIC,
	DOLLAR,
	HASH,
	COLON,
	INT,
	SYMBOL,
	LABEL,
	Eof,
	ILLEGAL
} Token_t;

typedef struct {
	char *literal;
	Token_t type;
} Token;

typedef struct {
	char *input;
	// the character literal under examination
	char ch;
	// the look ahead
	int read_position;
	// the anchor for identifiers and numbers
	int position;
} Lexer;

Lexer lexer_init(char *input);
Token *lex(Lexer *self);
Token next_token(Lexer *self);

void skip_whitespace(Lexer *self);
