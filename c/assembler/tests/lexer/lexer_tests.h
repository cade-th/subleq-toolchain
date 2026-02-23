#pragma once

#include "../tests.h"
#include "../../src/lexer/lexer.h"

void run_lexer_tests();

char* token_type_to_str(Token_t type);
bool compare_tokens(int index, Token expected, Token actual);
