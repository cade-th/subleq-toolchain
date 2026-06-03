import os
import sys
import json

# Add parent directory to sys.path for imports
sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))

from assembler import pass1, assemble
import tests.tester as tester

def pass1_test(test_name, data):
    res = pass1(data["input"])
    success = True

    if res["pc"] != data["expected"]["pc"]:
        print(f"PC mismatch in {test_name}: Expected {data['expected']['pc']}, got {res['pc']}")
        success = False

    if "symbols" in data["expected"]:
        for label, val in data["expected"]["symbols"].items():
            if res["symbols"].get(label) != val:
                print(f"Symbol mismatch for {label} in {test_name}: Expected {val}, got {res['symbols'].get(label)}")
                success = False

    if "macros" in data["expected"]:
        for macro_name, meta in data["expected"]["macros"].items():
            macro = res["macros"].get(macro_name)
            if not macro:
                print(f"Macro {macro_name} not found in {test_name}")
                success = False
            elif macro["argc"] != meta["argc"]:
                print(f"Macro {macro_name} argc mismatch in {test_name}: Expected {meta['argc']}, got {macro['argc']}")
                success = False

    if "tokens" in data["expected"]:
        if len(res["tokens"]) != len(data["expected"]["tokens"]):
            print(f"Tokens length mismatch in {test_name}: Expected {len(data['expected']['tokens'])}, got {len(res['tokens'])}")
            success = False
        else:
            for i in range(len(res["tokens"])):
                if res["tokens"][i]["type"] != data["expected"]["tokens"][i]["type"] or \
                   res["tokens"][i]["value"] != data["expected"]["tokens"][i]["value"]:
                    print(f"Token mismatch at {i} in {test_name}: Expected {data['expected']['tokens'][i]['type']}:{data['expected']['tokens'][i]['value']}, got {res['tokens'][i]['type']}:{res['tokens'][i]['value']}")
                    success = False
    return success

def pass2_test(test_name, data):
    try:
        binary = assemble(data["input"])
        if len(binary) != len(data["expected"]):
            print(f"Binary length mismatch in {test_name}: Expected {len(data['expected'])}, got {len(binary)}")
            return False
        for i in range(len(binary)):
            if binary[i] != data["expected"][i]:
                print(f"Binary mismatch at index {i} in {test_name}: Expected {data['expected'][i]}, got {binary[i]}")
                return False
        return True
    except Exception as e:
        print(f"Assembly failed in {test_name}: {e}")
        return False

if __name__ == "__main__":
    dir_path = os.path.dirname(__file__)
    with open(os.path.join(dir_path, "pass1_data.json"), "r") as f:
        pass1_data = json.load(f)
    with open(os.path.join(dir_path, "pass2_data.json"), "r") as f:
        pass2_data = json.load(f)

    tester.run("Assembler Pass 1 Tests", pass1_data, pass1_test)
    tester.run("Assembler Pass 2 Tests", pass2_data, pass2_test)
