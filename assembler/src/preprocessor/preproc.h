#pragma once
#include "../lexer/lexer.h"
#include "../util/hashmap.h"
#include "../util/dyn_array.h"

typedef struct {
	Token value;
	int position;
} Parameter;

typedef struct {
	char *key;
	int argc;
	Parameter *parameters;
	int mdt_index;
} MNT_Entry;

typedef struct {
	Token *input;	
	// This is a hashmap
	MNT_Entry *mnt[HASH_CAPACITY];
	// This is a 2D array of Tokens
	Token *mdt[256];
	int macro_count;
} Preproc;

Preproc preproc_init(Token *input);
void preprocess(Preproc *self);
