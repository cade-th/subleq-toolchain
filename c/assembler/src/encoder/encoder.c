#include "encoder.h"
#include <stdint.h>
#include "../util/dyn_array.h"
#include <stdlib.h>

Encoder encoder_init(int32_t *input) {
	Encoder encoder = {
		.input = input,
		.output = 0
	};
	return encoder;
}

int32_t *encode(Encoder *self, int instruction_cnt) {
	int32_t *output = DYN_ARRAY(int32_t);
	// In the flat model, input is already the machine code (ints)
    // So we just copy it. In a real scenario, this step might write to a binary file
    // or handle endianness, but for now we just pass it through.
	for (int i=0; i < instruction_cnt; i++) {
		ARRAY_PUSH(output, self->input[i]);
	}
	return output;
}
