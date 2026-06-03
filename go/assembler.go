package subleq

import (
	"fmt"
	"strconv"
	"strings"
)

type Macro struct {
	Params []string `json:"params"`
	Body   []Token  `json:"body"`
	Argc   int      `json:"argc"`
}

type Pass1Result struct {
	Macros  map[string]Macro `json:"macros"`
	Symbols map[string]int   `json:"symbols"`
	Tokens  []Token          `json:"tokens"`
	PC      int              `json:"pc"`
}

func parseDefArgs(tokens []Token, pos int) ([]string, int) {
	var args []string
	if pos < len(tokens) && tokens[pos].Type == "LPAREN" {
		pos++
		for pos < len(tokens) && tokens[pos].Type != "RPAREN" {
			if tokens[pos].Type == "SYMBOL" {
				args = append(args, tokens[pos].Value)
			}
			pos++
			if pos < len(tokens) && tokens[pos].Type == "COMMA" {
				pos++
			}
		}
		if pos < len(tokens) && tokens[pos].Type == "RPAREN" {
			pos++
		}
	}
	return args, pos
}

func collectCallArgs(tokens []Token, pos int, argc int) ([][]Token, int) {
	var args [][]Token
	if pos < len(tokens) && tokens[pos].Type == "LPAREN" {
		pos++
		for a := 0; a < argc; a++ {
			var argTokens []Token
			for pos < len(tokens) && tokens[pos].Type != "COMMA" && tokens[pos].Type != "RPAREN" {
				argTokens = append(argTokens, tokens[pos])
				pos++
			}
			args = append(args, argTokens)
			if pos < len(tokens) && tokens[pos].Type == "COMMA" {
				pos++
			}
		}
		if pos < len(tokens) && tokens[pos].Type == "RPAREN" {
			pos++
		}
		return args, pos
	}
	return nil, pos
}

func substitute(body []Token, params []string, args [][]Token) []Token {
	var result []Token
	for _, bodyToken := range body {
		substituted := false
		if bodyToken.Type == "SYMBOL" {
			for idx, paramName := range params {
				if bodyToken.Value == paramName {
					if idx < len(args) {
						result = append(result, args[idx]...)
					}
					substituted = true
					break
				}
			}
		}
		if !substituted {
			result = append(result, bodyToken)
		}
	}
	return result
}

func expand(tokens []Token, macros map[string]Macro, symbols map[string]int, output *[]Token, pc int) (int, error) {
	i := 0
	for i < len(tokens) {
		t := tokens[i]

		if t.Type == "HASH" && (i+1) < len(tokens) && tokens[i+1].Value == "define" {
			name := tokens[i+2].Value
			params, nextPos := parseDefArgs(tokens, i+3)
			var body []Token
			for nextPos < len(tokens) {
				bt := tokens[nextPos]
				if bt.Type == "SYMBOL" && strings.ToLower(bt.Value) == "endm" {
					nextPos++
					break
				}
				body = append(body, bt)
				nextPos++
			}
			macros[name] = Macro{Params: params, Body: body, Argc: len(params)}
			i = nextPos
		} else if t.Type == "LABEL" {
			symbols[t.Value] = pc
			i++
		} else if t.Type == "SYMBOL" {
			if macro, ok := macros[t.Value]; ok {
				args, nextPos := collectCallArgs(tokens, i+1, macro.Argc)
				if args != nil || macro.Argc == 0 {
					substituted := substitute(macro.Body, macro.Params, args)
					var err error
					pc, err = expand(substituted, macros, symbols, output, pc)
					if err != nil {
						return 0, err
					}
					i = nextPos
				} else {
					*output = append(*output, t)
					pc++
					i++
				}
			} else {
				*output = append(*output, t)
				pc++
				i++
			}
		} else if t.Type == "NUMBER" {
			*output = append(*output, t)
			pc++
			i++
		} else {
			i++
		}
	}
	return pc, nil
}

func Pass1(source interface{}) (*Pass1Result, error) {
	var tokens []Token
	var err error
	if s, ok := source.(string); ok {
		tokens, err = Tokenize(s)
		if err != nil {
			return nil, err
		}
	} else if t, ok := source.([]Token); ok {
		tokens = t
	} else {
		return nil, fmt.Errorf("source must be string or []Token")
	}

	macros := make(map[string]Macro)
	symbols := make(map[string]int)
	var expanded []Token
	pc, err := expand(tokens, macros, symbols, &expanded, 0)
	if err != nil {
		return nil, err
	}

	return &Pass1Result{
		Macros:  macros,
		Symbols: symbols,
		Tokens:  expanded,
		PC:      pc,
	}, nil
}

func Assemble(source string) ([]int, error) {
	res, err := Pass1(source)
	if err != nil {
		return nil, err
	}

	var binary []int
	for _, t := range res.Tokens {
		if t.Type == "SYMBOL" {
			if addr, ok := res.Symbols[t.Value]; ok {
				binary = append(binary, addr)
			} else {
				return nil, fmt.Errorf("undefined label: %s", t.Value)
			}
		} else if t.Type == "NUMBER" {
			val, _ := strconv.ParseFloat(t.Value, 64)
			binary = append(binary, int(val))
		}
	}

	return binary, nil
}
