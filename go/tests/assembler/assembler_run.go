package main

import (
	"fmt"
	"os"
	"subleq"
	tests "subleq/tests/tester"
)

func pass1Test(name string, v interface{}) bool {
	data := v.(map[string]interface{})
	input := data["input"].(string)
	expected := data["expected"].(map[string]interface{})

	res, err := subleq.Pass1(input)
	if err != nil {
		fmt.Printf("Pass1 error in %s: %v\n", name, err)
		return false
	}

	if res.PC != int(expected["pc"].(float64)) {
		fmt.Printf("PC mismatch in %s: Expected %d, got %d\n", name, int(expected["pc"].(float64)), res.PC)
		return false
	}

	if expectedSymbols, ok := expected["symbols"].(map[string]interface{}); ok {
		for label, val := range expectedSymbols {
			if res.Symbols[label] != int(val.(float64)) {
				fmt.Printf("Symbol mismatch for %s in %s: Expected %d, got %d\n",
					label, name, int(val.(float64)), res.Symbols[label])
				return false
			}
		}
	}

	if expectedMacros, ok := expected["macros"].(map[string]interface{}); ok {
		for macroName, meta := range expectedMacros {
			macro, ok := res.Macros[macroName]
			if !ok {
				fmt.Printf("Macro %s not found in %s\n", macroName, name)
				return false
			} else if macro.Argc != int(meta.(map[string]interface{})["argc"].(float64)) {
				fmt.Printf("Macro %s argc mismatch in %s: Expected %d, got %d\n",
					macroName, name, int(meta.(map[string]interface{})["argc"].(float64)), macro.Argc)
				return false
			}
		}
	}

	if expectedTokens, ok := expected["tokens"].([]interface{}); ok {
		if len(res.Tokens) != len(expectedTokens) {
			fmt.Printf("Token count mismatch in %s: Expected %d, got %d\n", name, len(expectedTokens), len(res.Tokens))
			return false
		}
		for i, tokenData := range expectedTokens {
			expectedToken := tokenData.(map[string]interface{})
			if res.Tokens[i].Type != expectedToken["type"].(string) || res.Tokens[i].Value != expectedToken["value"].(string) {
				fmt.Printf("Token mismatch at %d in %s: Expected %s:%s, got %s:%s\n",
					i, name, expectedToken["type"].(string), expectedToken["value"].(string), res.Tokens[i].Type, res.Tokens[i].Value)
				return false
			}
		}
	}

	return true
}

func pass2Test(name string, v interface{}) bool {
	data := v.(map[string]interface{})
	input := data["input"].(string)
	expected := data["expected"].([]interface{})

	binary, err := subleq.Assemble(input)
	if err != nil {
		fmt.Printf("Assembly error in %s: %v\n", name, err)
		return false
	}

	if len(binary) != len(expected) {
		fmt.Printf("Binary length mismatch in %s: Expected %d, got %d\n", name, len(expected), len(binary))
		return false
	}

	for i, val := range expected {
		if binary[i] != int(val.(float64)) {
			fmt.Printf("Binary mismatch at index %d in %s: Expected %d, got %d\n",
				i, name, int(val.(float64)), binary[i])
			return false
		}
	}

	return true
}

func main() {
	pass1Data, err := tests.LoadJSON("pass1_data.json")
	if err != nil {
		fmt.Printf("Could not load pass1 test data: %v\n", err)
		os.Exit(1)
	}
	tests.Run("Assembler Pass 1 Tests", pass1Data, pass1Test)

	pass2Data, err := tests.LoadJSON("pass2_data.json")
	if err != nil {
		fmt.Printf("Could not load pass2 test data: %v\n", err)
		os.Exit(1)
	}
	tests.Run("Assembler Pass 2 Tests", pass2Data, pass2Test)
}
