import os
import sys
import json
import io

# Add parent directory to sys.path for imports
sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))

from emu.cpu import CPU
import tests.tester as tester

def emu_test(test_name, data):
    cpu = CPU()
    cpu.load_program(data["program"])
    
    output_buffer = io.StringIO()
    original_stdout = sys.stdout
    sys.stdout = output_buffer

    # Safety cycle limit
    cycles = data.get("max_cycles", 100)
    cpu.run(cycles)

    sys.stdout = original_stdout
    output = output_buffer.getvalue()

    success = True

    # Check Output
    if "expected_output" in data and output != data["expected_output"]:
        print(f"Output mismatch in {test_name}: Expected {repr(data['expected_output'])}, got {repr(output)}")
        success = False

    # Check PC
    if "expected_pc" in data and cpu.pc != data.get("expected_pc"):
        print(f"PC mismatch in {test_name}: Expected {data['expected_pc']}, got {cpu.pc}")
        success = False

    # Check Memory
    if "expected_memory" in data:
        for addr_str, expected_val in data["expected_memory"].items():
            addr = int(addr_str)
            if cpu.memory[addr] != expected_val:
                print(f"Memory mismatch at address {addr} in {test_name}: Expected {expected_val}, got {cpu.memory[addr]}")
                success = False

    return success

if __name__ == "__main__":
    with open(os.path.join(os.path.dirname(__file__), "emu_test_data.json"), "r") as f:
        test_data = json.load(f)
    tester.run("Emulator Tests", test_data, emu_test)
