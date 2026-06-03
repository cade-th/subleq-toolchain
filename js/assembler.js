const lexer = require('./lexer');

const parseDefArgs = (tokens, pos) => {
    let args = [];
    if (tokens[pos] && tokens[pos].type === 'LPAREN') {
        pos++;
        while (tokens[pos] && tokens[pos].type !== 'RPAREN') {
            if (tokens[pos].type === 'SYMBOL') {
                args.push(tokens[pos].value);
            }
            pos++;
            if (tokens[pos] && tokens[pos].type === 'COMMA') {
                pos++;
            }
        }
        if (tokens[pos] && tokens[pos].type === 'RPAREN') {
            pos++;
        }
    }
    return { args, pos };
};

const collectCallArgs = (tokens, pos, argc) => {
    let args = [];
    if (tokens[pos] && tokens[pos].type === 'LPAREN') {
        pos++;
        for (let a = 0; a < argc; a++) {
            let argTokens = [];
            while (tokens[pos] && tokens[pos].type !== 'COMMA' && tokens[pos].type !== 'RPAREN') {
                argTokens.push(tokens[pos]);
                pos++;
            }
            args.push(argTokens);
            if (tokens[pos] && tokens[pos].type === 'COMMA') {
                pos++;
            }
        }
        if (tokens[pos] && tokens[pos].type === 'RPAREN') {
            pos++;
        }
        return { args, pos };
    }
    return { args: null, pos };
};

const substitute = (body, params, args) => {
    let result = [];
    for (const bodyToken of body) {
        let substituted = false;
        if (bodyToken.type === 'SYMBOL') {
            for (let idx = 0; idx < params.length; idx++) {
                if (bodyToken.value === params[idx]) {
                    if (args[idx]) {
                        result.push(...args[idx]);
                    }
                    substituted = true;
                    break;
                }
            }
        }
        if (!substituted) {
            result.push(bodyToken);
        }
    }
    return result;
};

const expand = (tokens, macros, symbols, output, pc) => {
    let i = 0;
    while (i < tokens.length) {
        const t = tokens[i];

        if (t.type === 'HASH' && tokens[i + 1] && tokens[i + 1].value === 'define') {
            const name = tokens[i + 2].value;
            const { args: params, pos: next_pos_initial } = parseDefArgs(tokens, i + 3);
            let next_pos = next_pos_initial;
            let body = [];
            while (next_pos < tokens.length) {
                const bt = tokens[next_pos];
                if (bt.type === 'SYMBOL' && bt.value.toLowerCase() === 'endm') {
                    next_pos++;
                    break;
                }
                body.push(bt);
                next_pos++;
            }
            macros[name] = { params, body, argc: params.length };
            i = next_pos;
        } else if (t.type === 'LABEL') {
            symbols[t.value] = pc;
            i++;
        } else if (t.type === 'SYMBOL' && macros[t.value]) {
            const macro = macros[t.value];
            const { args, pos: next_pos } = collectCallArgs(tokens, i + 1, macro.argc);
            if (args !== null || macro.argc === 0) {
                const substituted = substitute(macro.body, macro.params, args || []);
                pc = expand(substituted, macros, symbols, output, pc);
                i = next_pos;
            } else {
                output.push(t);
                pc++;
                i++;
            }
        } else if (t.type === 'NUMBER' || t.type === 'SYMBOL') {
            output.push(t);
            pc++;
            i++;
        } else {
            i++;
        }
    }
    return pc;
};

const pass1 = (source) => {
    const tokens = (typeof source === 'string') ? lexer(source) : source;
    let macros = {}, symbols = {}, expanded = [];
    let pc = expand(tokens, macros, symbols, expanded, 0);
    return {
        macros,
        symbols,
        tokens: expanded,
        pc
    };
};

const assemble = (source) => {
    const res = pass1(source);
    let binary = [];
    for (const t of res.tokens) {
        if (t.type === 'SYMBOL') {
            const addr = res.symbols[t.value];
            if (addr === undefined) throw new Error(`Undefined label: ${t.value}`);
            binary.push(addr);
        } else if (t.type === 'NUMBER') {
            binary.push(Number(t.value));
        }
    }
    return binary;
};

module.exports = {
    assemble,
    pass1
};
