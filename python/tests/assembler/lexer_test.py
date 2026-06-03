import os
import sys
import json

# Add parent directory to sys.path for imports
sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))

from lexer import tokenize
import tests.tester as tester

def lexer_test(test_name, data):
    output = tokenize(data["input"])
    for i in range(len(output)):
        token = output[i]
        if i >= len(data["expected"]):
            print(f"Extra token at index {i} in {test_name}: Got {token['type']}:{token['value']}")
            return False
        
        expected = data["expected"][i]
        if token["type"] != expected["type"] or token["value"] != expected["value"]:
            print(f"Mismatch at index {i} in {test_name}: Expected {expected['type']}:{expected['value']}, got {token['type']}:{token['value']}")
            return False
            
    if len(output) < len(data["expected"]):
        print(f"Missing tokens in {test_name}: Expected {len(data['expected'])}, got {len(output)}")
        return False
        
    return True

if __name__ == "__main__":
    with open(os.path.join(os.path.dirname(__file__), "lexer_data.json"), "r") as f:
        test_data = json.load(f)
    tester.run("Lexer Tests", test_data, lexer_test)
