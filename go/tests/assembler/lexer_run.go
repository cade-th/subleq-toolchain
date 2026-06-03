package main

import (
	"fmt"
	"os"
	"subleq"
	tests "subleq/tests/tester"
)

func lexerTest(name string, data interface{}) bool {
	testData := data.(map[string]interface{})
	input := testData["input"].(string)
	expected := testData["expected"].([]interface{})

	output, err := subleq.Tokenize(input)
	if err != nil {
		fmt.Printf("Lexer error in %s: %v\n", name, err)
		return false
	}

	if len(output) != len(expected) {
		fmt.Printf("Token count mismatch in %s: Expected %d, got %d\n", name, len(expected), len(output))
		return false
	}

	for i, tokenData := range expected {
		expectedToken := tokenData.(map[string]interface{})
		if output[i].Type != expectedToken["type"].(string) || output[i].Value != expectedToken["value"].(string) {
			fmt.Printf("Mismatch at index %d in %s: Expected %s:%s, got %s:%s\n",
				i, name, expectedToken["type"].(string), expectedToken["value"].(string), output[i].Type, output[i].Value)
			return false
		}
	}

	return true
}

func main() {
	testData, err := tests.LoadJSON("lexer_data.json")
	if err != nil {
		fmt.Printf("Could not load test data: %v\n", err)
		os.Exit(1)
	}

	tests.Run("Lexer Tests", testData, lexerTest)
}
