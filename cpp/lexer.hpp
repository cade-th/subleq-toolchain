#ifndef SUBLEQ_LEXER_HPP
#define SUBLEQ_LEXER_HPP

#include <string>
#include <vector>

struct Token {
    std::string type;
    std::string value;
};

std::vector<Token> tokenize(const std::string& input);

#endif
