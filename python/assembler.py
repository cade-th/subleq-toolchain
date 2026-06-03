from lexer import tokenize

def parse_def_args(tokens, pos):
    args = []
    if pos < len(tokens) and tokens[pos]["type"] == 'LPAREN':
        pos += 1
        while pos < len(tokens) and tokens[pos]["type"] != 'RPAREN':
            if tokens[pos]["type"] == 'SYMBOL':
                args.append(tokens[pos]["value"])
            pos += 1
            if pos < len(tokens) and tokens[pos]["type"] == 'COMMA':
                pos += 1
        if pos < len(tokens) and tokens[pos]["type"] == 'RPAREN':
            pos += 1
    return args, pos

def collect_call_args(tokens, pos, argc):
    args = []
    if pos < len(tokens) and tokens[pos]["type"] == 'LPAREN':
        pos += 1
        for _ in range(argc):
            arg_tokens = []
            while pos < len(tokens) and tokens[pos]["type"] != 'COMMA' and tokens[pos]["type"] != 'RPAREN':
                arg_tokens.append(tokens[pos])
                pos += 1
            args.append(arg_tokens)
            if pos < len(tokens) and tokens[pos]["type"] == 'COMMA':
                pos += 1
        if pos < len(tokens) and tokens[pos]["type"] == 'RPAREN':
            pos += 1
        return args, pos
    return None, pos

def substitute(body, params, args):
    result = []
    for body_token in body:
        substituted = False
        if body_token["type"] == 'SYMBOL':
            for idx, param_name in enumerate(params):
                if body_token["value"] == param_name:
                    if idx < len(args):
                        result.extend(args[idx])
                    substituted = True
                    break
        if not substituted:
            result.append(body_token)
    return result

def expand(tokens, macros, symbols, output, pc):
    i = 0
    while i < len(tokens):
        t = tokens[i]

        if t["type"] == 'HASH' and (i + 1) < len(tokens) and tokens[i + 1]["value"] == 'define':
            name = tokens[i + 2]["value"]
            params, next_pos = parse_def_args(tokens, i + 3)
            body = []
            while next_pos < len(tokens):
                bt = tokens[next_pos]
                if bt["type"] == 'SYMBOL' and bt["value"].lower() == 'endm':
                    next_pos += 1
                    break
                body.append(bt)
                next_pos += 1
            macros[name] = {"params": params, "body": body, "argc": len(params)}
            i = next_pos
        elif t["type"] == 'LABEL':
            symbols[t["value"]] = pc
            i += 1
        elif t["type"] == 'SYMBOL' and t["value"] in macros:
            macro = macros[t["value"]]
            args, next_pos = collect_call_args(tokens, i + 1, macro["argc"])
            if args is not None or macro["argc"] == 0:
                substituted = substitute(macro["body"], macro["params"], args or [])
                pc = expand(substituted, macros, symbols, output, pc)
                i = next_pos
            else:
                output.append(t)
                pc += 1
                i += 1
        elif t["type"] == 'NUMBER' or t["type"] == 'SYMBOL':
            output.append(t)
            pc += 1
            i += 1
        else:
            i += 1
    return pc

def pass1(source):
    tokens = tokenize(source) if isinstance(source, str) else source
    macros, symbols, expanded = {}, {}, []
    pc = expand(tokens, macros, symbols, expanded, 0)
    return {
        "macros": macros,
        "symbols": symbols,
        "tokens": expanded,
        "pc": pc
    }

def assemble(source):
    res = pass1(source)
    binary = []
    for t in res["tokens"]:
        if t["type"] == 'SYMBOL':
            addr = res["symbols"].get(t["value"])
            if addr is None:
                raise Exception(f"Undefined label: {t['value']}")
            binary.append(addr)
        elif t["type"] == 'NUMBER':
            binary.append(int(float(t["value"])))
    return binary
