#include "assembler.hpp"
#include <algorithm>
#include <stdexcept>
#include <iostream>

static std::pair<std::vector<std::string>, size_t> parse_def_args(const std::vector<Token>& tokens, size_t pos) {
    std::vector<std::string> args;
    if (pos < tokens.size() && tokens[pos].type == "LPAREN") {
        pos++;
        while (pos < tokens.size() && tokens[pos].type != "RPAREN") {
            if (tokens[pos].type == "SYMBOL") {
                args.push_back(tokens[pos].value);
            }
            pos++;
            if (pos < tokens.size() && tokens[pos].type == "COMMA") {
                pos++;
            }
        }
        if (pos < tokens.size() && tokens[pos].type == "RPAREN") {
            pos++;
        }
    }
    return {args, pos};
}

static std::pair<std::vector<std::vector<Token>>, size_t> collect_call_args(const std::vector<Token>& tokens, size_t pos, int argc) {
    std::vector<std::vector<Token>> args;
    if (pos < tokens.size() && tokens[pos].type == "LPAREN") {
        pos++;
        for (int a = 0; a < argc; a++) {
            std::vector<Token> arg_tokens;
            while (pos < tokens.size() && tokens[pos].type != "COMMA" && tokens[pos].type != "RPAREN") {
                arg_tokens.push_back(tokens[pos]);
                pos++;
            }
            args.push_back(arg_tokens);
            if (pos < tokens.size() && tokens[pos].type == "COMMA") {
                pos++;
            }
        }
        if (pos < tokens.size() && tokens[pos].type == "RPAREN") {
            pos++;
        }
        return {args, pos};
    }
    return {{}, pos};
}

static std::vector<Token> substitute(const std::vector<Token>& body, const std::vector<std::string>& params, const std::vector<std::vector<Token>>& args) {
    std::vector<Token> result;
    for (const auto& body_token : body) {
        bool substituted = false;
        if (body_token.type == "SYMBOL") {
            for (size_t idx = 0; idx < params.size(); idx++) {
                if (body_token.value == params[idx]) {
                    if (idx < args.size()) {
                        result.insert(result.end(), args[idx].begin(), args[idx].end());
                    }
                    substituted = true;
                    break;
                }
            }
        }
        if (!substituted) {
            result.push_back(body_token);
        }
    }
    return result;
}

static int expand(const std::vector<Token>& tokens, std::map<std::string, Macro>& macros, std::map<std::string, int>& symbols, std::vector<Token>& output, int pc) {
    size_t i = 0;
    while (i < tokens.size()) {
        const auto& t = tokens[i];

        if (t.type == "HASH" && (i + 1) < tokens.size() && tokens[i + 1].value == "define") {
            std::string name = tokens[i + 2].value;
            auto res = parse_def_args(tokens, i + 3);
            std::vector<std::string> params = res.first;
            size_t next_pos = res.second;
            std::vector<Token> body;
            while (next_pos < tokens.size()) {
                const auto& bt = tokens[next_pos];
                // Case insensitive check
                std::string lvalue = bt.value;
                std::transform(lvalue.begin(), lvalue.end(), lvalue.begin(), ::tolower);
                if (bt.type == "SYMBOL" && lvalue == "endm") {
                    next_pos++;
                    break;
                }
                body.push_back(bt);
                next_pos++;
            }
            macros[name] = {params, body, (int)params.size()};
            i = next_pos;
        } else if (t.type == "LABEL") {
            symbols[t.value] = pc;
            i++;
        } else if (t.type == "SYMBOL" && macros.count(t.value)) {
            const auto& macro = macros[t.value];
            auto res = collect_call_args(tokens, i + 1, macro.argc);
            // Collect call args returns empty vector but next_pos might change if () were present.
            // Simplified logic: اگر macro.argc > 0 we expect parens. If argc == 0 parens optional.
            // We'll follow the same pattern as JS/Python.
            // But wait, the JS/Python version returns null for args if LPAREN not found.
            // Let's adjust collect_call_args.
            bool had_parens = (i + 1 < tokens.size() && tokens[i + 1].type == "LPAREN");

            if (had_parens || macro.argc == 0) {
                pc = expand(substitute(macro.body, macro.params, res.first), macros, symbols, output, pc);
                i = res.second;
            } else {
                output.push_back(t);
                pc++;
                i++;
            }
        } else if (t.type == "NUMBER" || t.type == "SYMBOL") {
            output.push_back(t);
            pc++;
            i++;
        } else {
            i++;
        }
    }
    return pc;
}

Pass1Result pass1(const std::string& source) {
    return pass1(tokenize(source));
}

Pass1Result pass1(const std::vector<Token>& tokens) {
    Pass1Result res;
    res.pc = expand(tokens, res.macros, res.symbols, res.tokens, 0);
    return res;
}

std::vector<int> assemble(const std::string& source) {
    Pass1Result res = pass1(source);
    std::vector<int> binary;
    for (const auto& t : res.tokens) {
        if (t.type == "SYMBOL") {
            if (res.symbols.count(t.value)) {
                binary.push_back(res.symbols[t.value]);
            } else {
                throw std::runtime_error("Undefined label: " + t.value);
            }
        } else if (t.type == "NUMBER") {
            binary.push_back(std::stoi(t.value));
        }
    }
    return binary;
}
