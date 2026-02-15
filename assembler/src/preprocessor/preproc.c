#include "preproc.h"
#include <stdio.h>
#include <string.h>
#include "../util/hashmap.h"

Preproc preproc_init(Token *input) {
	Preproc preproc = {
		.input = input,
		// make this a hash map probably
		.mnt = { 0 },
		.mdt = { 0 },
		.macro_count = 0,
	};
	return preproc;
}

// This all pretty much works but I need to split it into differnt functions
void preprocess(Preproc *self) {
	// loop through the tokens to find #define
	int temp = 0;
	#define CURRENT self->input[temp]
	#define NEXT self->input[temp + 1]
	#define NEXT_NEXT self->input[temp + 2]

	while (CURRENT.type != Eof) {
		if (CURRENT.type == HASH) {
			self->mdt[self->macro_count] = DYN_ARRAY(Token);
			// Changing this if we want macros to have dependencies via ifdef etc
			if (strcmp(NEXT.literal, "define") == 0){

				// record the macro in the MNT
				MNT_Entry new = {
					.key = NEXT_NEXT.literal,
					.mdt_index = self->macro_count
				};
				// put the inner pointer ahead of the macro identifer
				int temp2 = temp +=3;
				#define INNER_CURRENT self->input[temp2]
				while (strcmp(INNER_CURRENT.literal, "endM") != 0) {
					// now in this loop we can push tokens into shit
					ARRAY_PUSH(self->mdt[self->macro_count], self->input[temp2]);
					// printf("Token Added\n");
					temp2++;	
				}
				// Push the macro into the MNT
				HASH_PUSH(self->mnt, new.key, new, MNT_Entry);
				// printf("macro found!\n");
			}
			// we know we're in a macro now

		self->macro_count++;
		}
		temp++;
		
	}
}


