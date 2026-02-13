#pragma once
#include <stdint.h>

#include "../../src/lexer/lexer.h"
#include "../../src/parser/parser.h"
#include "../../src/encoder/encoder.h"

Token *tokenize_api(char *src_code);

ParsedInstruction *parse_api(char *src_code);

uint8_t *encode_api(char *src_code);

void assemble_api(char *src_code);
