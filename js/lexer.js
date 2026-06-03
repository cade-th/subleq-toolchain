const token_definitions = [
    { type: "NUMBER", pattern: /^-?\d+\.?\d*/ },
    { type: "HASH", pattern: /^#/ },
    { type: "LABEL", pattern: /^([a-zA-Z_][a-zA-Z0-9_]*):/ },
    { type: "SYMBOL", pattern: /^[a-zA-Z_][a-zA-Z0-9_]*/ },
    { type: "LPAREN", pattern: /^\(/ },
    { type: "RPAREN", pattern: /^\)/ },
    { type: "COMMA", pattern: /^,/ },
    { type: "WHITESPACE", pattern: /^\s+/ },
];

function tokenize(input) {
    const tokens = [];
    let pos = 0;

    while (pos < input.length) {
        let matched = false;
        const substring = input.slice(pos);

        for (const def of token_definitions) {
            const match = substring.match(def.pattern);

            if (match) {
                if (def.type !== "WHITESPACE") {
                    // Capture the group if it exists, otherwise the whole match
                    const value = match[1] || match[0];
                    tokens.push({ type: def.type, value: value });
                }

                pos += match[0].length;
                matched = true;
                break;
            }
        }

        if (!matched) {
            throw new Error(`Unexpected character at position ${pos}: ${input[pos]}`);
        }
    }

    return tokens;
}

module.exports = tokenize;
