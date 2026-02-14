#include "preproc.h"
#include <stdio.h>
#include <string.h>
#include "../util/dyn_array.h"

Preproc preproc_init(Token *input) {
	Preproc preproc = {
		.input = input,
		// make this a hash map probably
		.mnt = DYN_ARRAY(MNT_Entry)
	};
	return preproc;
}

void preprocess(Preproc *self) {
	// loop through the tokens to find #define
	int temp = 0;
	#define CURRENT self->input[temp]
	#define NEXT self->input[temp + 1]
	#define NEXT_NEXT self->input[temp + 2]

	while (CURRENT.type != Eof) {
		if (CURRENT.type == HASH) {
			// Changing this if we want macros to have dependencies via ifdef etc
			if (strcmp(NEXT.literal, "define") == 0){
				MNT_Entry new = {
					.name = NEXT_NEXT.literal
				};
				ARRAY_PUSH(self->mnt, new); 
				printf("macro found!\n");
			}
			// we know we're in a macro now
		}
		temp++;
		
	}
}


