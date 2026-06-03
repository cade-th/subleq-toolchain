package main

import (
	"bytes"
	"fmt"
	"os"
	"subleq"
	"subleq/emu"
	tests "subleq/tests/tester"
)

func e2eTest(name string, v interface{}) bool {
	data := v.(map[string]interface{})
	input := data["input"].(string)

	binary, err := subleq.Assemble(input)
	if err != nil {
		fmt.Printf("Assembly error in %s: %v\n", name, err)
		return false
	}

	cpu := emu.NewCPU()
	cpu.LoadProgram(binary)

	// Mock output
	var outputBuffer bytes.Buffer
	oldStdout := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Safety cycle limit
	cycles := 1000
	if val, ok := data["max_cycles"].(float64); ok {
		cycles = int(val)
	}
	cpu.Run(cycles)

	w.Close()
	outputBuffer.ReadFrom(r)
	os.Stdout = oldStdout
	output := outputBuffer.String()

	success := true

	if expectedVal, ok := data["expected_output"].(string); ok {
		if output != expectedVal {
			fmt.Printf("Output mismatch in %s: Expected %q, got %q\n", name, expectedVal, output)
			success = false
		}
	}

	if expectedPC, ok := data["expected_pc"].(float64); ok {
		if cpu.PC != int(expectedPC) {
			fmt.Printf("PC mismatch in %s: Expected %d, got %d\n", name, int(expectedPC), cpu.PC)
			success = false
		}
	}

	if expectedMemory, ok := data["expected_memory"].(map[string]interface{}); ok {
		for addrStr, expectedVal := range expectedMemory {
			var addr int
			fmt.Sscanf(addrStr, "%d", &addr)
			if cpu.Memory[addr] != int(expectedVal.(float64)) {
				fmt.Printf("Memory mismatch at address %d in %s: Expected %d, got %d\n",
					addr, name, int(expectedVal.(float64)), cpu.Memory[addr])
				success = false
			}
		}
	}

	return success
}

func main() {
	testData, err := tests.LoadJSON("end_to_end_data.json")
	if err != nil {
		fmt.Printf("Could not load e2e test data: %v\n", err)
		os.Exit(1)
	}

	tests.Run("End-to-End Tests", testData, e2eTest)
}
