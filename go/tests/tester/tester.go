package tests

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"os"
)

type TestCase map[string]interface{}

type TestSuite struct {
	Name     string
	Data     map[string]interface{}
	TestFunc func(string, interface{}) bool
}

func Run(suiteName string, testData map[string]interface{}, testFunc func(string, interface{}) bool) bool {
	success := true
	count := 0

	for name, data := range testData {
		count++
		ok := testFunc(name, data)
		if !ok {
			success = false
		}
	}

	if count == 0 {
		fmt.Printf("%s: NO TESTS FOUND\n", suiteName)
		return false
	}

	if success {
		fmt.Printf("%s: PASS\n", suiteName)
	} else {
		fmt.Printf("%s: FAIL\n", suiteName)
		os.Exit(1)
	}

	return success
}

func LoadJSON(path string) (map[string]interface{}, error) {
	data, err := ioutil.ReadFile(path)
	if err != nil {
		return nil, err
	}

	var testData map[string]interface{}
	err = json.Unmarshal(data, &testData)
	if err != nil {
		return nil, err
	}

	return testData, nil
}
