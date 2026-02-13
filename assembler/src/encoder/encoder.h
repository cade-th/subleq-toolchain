#pragma once
#include "../parser/parser.h"

typedef struct {
	ParsedInstruction *input;	
	char *output;
} Encoder;

Encoder encoder_init(ParsedInstruction *input);

uint8_t *encode(Encoder *self, int instruction_cnt);
