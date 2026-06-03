#include "lexer.hpp"
#include <regex>
#include <stdexcept>

struct TokenDef {
    std::string type;
    std::regex pattern;
    int group;
};

static const std::vector<TokenDef> token_definitions = {
    {"NUMBER",     std::regex(R"(^-?\d+\.?\d*)"), 0},
    {"HASH",       std::regex(R"(^#)"), 0},
    {"LABEL",      std::regex(R"(^([a-zA-Z_][a-zA-Z0-9_]*):)"), 1},
    {"SYMBOL",     std::regex(R"(^[a-zA-Z_][a-zA-Z0-9_]*)"), 0},
    {"LPAREN",     std::regex(R"(^\()"), 0},
    {"RPAREN",     std::regex(R"(^\))"), 0},
    {"COMMA",      std::regex(R"(^,)"), 0},
    {"WHITESPACE", std::regex(R"(^\s+)"), 0}
};

std::vector<Token> tokenize(const std::string& input) {
    std::vector<Token> tokens;
    size_t pos = 0;

    while (pos < input.length()) {
        bool matched = false;
        std::string substring = input.substr(pos);

        for (const auto& def : token_definitions) {
            std::smatch match;
            if (std::regex_search(substring, match, def.pattern)) {
                if (match.position() == 0) { // Should be 0 because of ^ in pattern, but let's be explicit
                    if (def.type != "WHITESPACE") {
                        std::string value = (def.group > 0 && match.size() > (size_t)def.group) 
                                            ? match[def.group].str() 
                                            : match[0].str();
                        tokens.push_back({def.type, value});
                    }
                    pos += match.length();
                    matched = true;
                    break;
                }
            }
        }

        if (!matched) {
            throw std::runtime_error("Unexpected character at position " + std::to_string(pos) + ": " + input[pos]);
        }
    }

    return tokens;
}
