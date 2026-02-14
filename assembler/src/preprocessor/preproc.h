#pragma once
#include "../lexer/lexer.h"

typedef struct {
	Token value;
	int position;
} Parameter;

typedef struct {
	char *name;
	int argc;
	Parameter *parameters;
	int mdt_index;
} MNT_Entry;

typedef struct {
	Token *input;	
	MNT_Entry *mnt;
} Preproc;

// MDT
// This will be the macro definition table that stores the body of the macro as an array of tokens, so an array of arrays
typedef struct {
	Token *tokens;
} Macro;

Preproc preproc_init(Token *input);
void preprocess(Preproc *self);
