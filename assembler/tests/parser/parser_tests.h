#pragma once
#include <stdbool.h>
#include <stdio.h>

#define TEST_CHECK(name) \
	do { \
		if (name()) { \
			printf("%-20s [\033[1;32m PASS \033[0m]\n", #name); \
		} else { \
			printf("%-20s [\033[1;31m FAIL \033[0m]\n", #name); \
	   	} \
	} while (0)


bool parser();

void run_parser_tests();
