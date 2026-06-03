package subleq

import (
	"fmt"
	"regexp"
)

type Token struct {
	Type  string `json:"type"`
	Value string `json:"value"`
}

type tokenDef struct {
	Type    string
	Pattern *regexp.Regexp
	Group   int
}

var tokenDefinitions = []tokenDef{
	{"NUMBER", regexp.MustCompile(`^-?\d+\.?\d*`), 0},
	{"HASH", regexp.MustCompile(`^#`), 0},
	{"LABEL", regexp.MustCompile(`^([a-zA-Z_][a-zA-Z0-9_]*):`), 1},
	{"SYMBOL", regexp.MustCompile(`^[a-zA-Z_][a-zA-Z0-9_]*`), 0},
	{"LPAREN", regexp.MustCompile(`^\(`), 0},
	{"RPAREN", regexp.MustCompile(`^\)`), 0},
	{"COMMA", regexp.MustCompile(`^,`), 0},
	{"WHITESPACE", regexp.MustCompile(`^\s+`), 0},
}

func Tokenize(input string) ([]Token, error) {
	var tokens []Token
	pos := 0

	for pos < len(input) {
		matched := false
		substring := input[pos:]

		for _, def := range tokenDefinitions {
			loc := def.Pattern.FindStringSubmatchIndex(substring)
			if loc != nil && loc[0] == 0 {
				if def.Type != "WHITESPACE" {
					value := ""
					if def.Group > 0 && len(loc) > (def.Group*2+1) {
						value = substring[loc[def.Group*2]:loc[def.Group*2+1]]
					} else {
						value = substring[loc[0]:loc[1]]
					}
					tokens = append(tokens, Token{Type: def.Type, Value: value})
				}
				pos += loc[1]
				matched = true
				break
			}
		}

		if !matched {
			return nil, fmt.Errorf("unexpected character at position %d: %c", pos, input[pos])
		}
	}

	return tokens, nil
}
