#include "encoder.h"
#include <stdint.h>
#include "../util/dyn_array.h"
#include <stdlib.h>

Encoder encoder_init(ParsedInstruction *input) {
	Encoder encoder = {
		.input = input,
		.output = 0
	};
	return encoder;
}

uint8_t *encode(Encoder *self, int instruction_cnt) {
	uint8_t *output = DYN_ARRAY(uint8_t);
	// is this right?
	// we need a way to determine how many parsed instructions there are here
	for (int i=0; i < instruction_cnt; i++) {
		ARRAY_PUSH(output, self->input[i].op_a);
		ARRAY_PUSH(output, self->input[i].op_b);
		ARRAY_PUSH(output, self->input[i].op_c);
	}
	return output;
}
