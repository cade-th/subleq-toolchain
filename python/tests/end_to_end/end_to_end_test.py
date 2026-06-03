import os
import sys
import json
import io

# Add parent directory to sys.path for imports
sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))

from assembler import assemble
from emu.cpu import CPU
import tests.tester as tester

def e2e_test(name, data):
    # Pipeline
    program = assemble(data["input"])
    cpu = CPU()
    cpu.load_program(program)

    output_buffer = io.StringIO()
    original_stdout = sys.stdout
    sys.stdout = output_buffer

    cpu.run(data.get("max_cycles", 1000))
    sys.stdout = original_stdout
    output = output_buffer.getvalue()

    success = True

    # Check Output
    if "expected_output" in data and output != data["expected_output"]:
        print(f"Output mismatch in {name}: Expected {repr(data['expected_output'])}, got {repr(output)}")
        success = False

    # Check Memory
    if "expected_memory" in data:
        for addr_str, expected_val in data["expected_memory"].items():
            addr = int(addr_str)
            if cpu.memory[addr] != expected_val:
                print(f"Memory mismatch in {name} at address {addr}: Expected {expected_val}, got {cpu.memory[addr]}")
                success = False

    # Check PC
    if "expected_pc" in data and cpu.pc != data.get("expected_pc"):
        print(f"PC mismatch in {name}: Expected {data['expected_pc']}, got {cpu.pc}")
        success = False

    return success

if __name__ == "__main__":
    with open(os.path.join(os.path.dirname(__file__), "end_to_end_data.json"), "r") as f:
        test_data = json.load(f)
    tester.run("End-to-End Tests", test_data, e2e_test)
