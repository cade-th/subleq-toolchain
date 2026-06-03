#ifndef SUBLEQ_ASSEMBLER_HPP
#define SUBLEQ_ASSEMBLER_HPP

#include "lexer.hpp"
#include <map>
#include <vector>
#include <string>

struct Macro {
    std::vector<std::string> params;
    std::vector<Token> body;
    int argc;
};

struct Pass1Result {
    std::map<std::string, Macro> macros;
    std::map<std::string, int> symbols;
    std::vector<Token> tokens;
    int pc;
};

Pass1Result pass1(const std::string& source);
Pass1Result pass1(const std::vector<Token>& tokens);
std::vector<int> assemble(const std::string& source);

#endif
