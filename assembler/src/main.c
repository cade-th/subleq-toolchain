#include <stdio.h>
#include "api/api.h"
#include "util/io.h"

int main(int argc, char *argv[]) {
	
	if (argc < 2) {
		printf("Usage: subleq [file path]\n");
		return 1;
	}

	File input = io_file_read(argv[1]);
	
	assemble_api(input.data);

	return 0;
}
