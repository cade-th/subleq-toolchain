import re

token_definitions = [
    ("NUMBER", r"^-?\d+\.?\d*"),
    ("HASH", r"^#"),
    ("LABEL", r"^([a-zA-Z_][a-zA-Z0-9_]*):"),
    ("SYMBOL", r"^[a-zA-Z_][a-zA-Z0-9_]*"),
    ("LPAREN", r"^\("),
    ("RPAREN", r"^\)"),
    ("COMMA", r"^,"),
    ("WHITESPACE", r"^\s+"),
]

def tokenize(input_str):
    tokens = []
    pos = 0

    while pos < len(input_str):
        matched = False
        substring = input_str[pos:]

        for type_name, pattern in token_definitions:
            match = re.search(pattern, substring)

            if match:
                if type_name != "WHITESPACE":
                    # Capture the group if it exists, otherwise the whole match
                    value = match.group(1) if match.groups() else match.group(0)
                    tokens.append({"type": type_name, "value": value})

                pos += match.end()
                matched = True
                break

        if not matched:
            raise Exception(f"Unexpected character at position {pos}: {input_str[pos]}")

    return tokens
