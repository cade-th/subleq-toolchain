#pragma once
#include "../parser/parser.h"

typedef struct {
	int32_t *input;	
	int32_t *output;
} Encoder;

Encoder encoder_init(int32_t *input);

int32_t *encode(Encoder *self, int instruction_cnt);
