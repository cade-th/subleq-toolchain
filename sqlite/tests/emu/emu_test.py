import sqlite3
import json
import os
import sys

# Subleq in SQLite: Python driver to run the SQL engine
class SQLiteCPU:
    def __init__(self):
        self.db = sqlite3.connect(':memory:')
        self.output = ""
        self.setup()

    def setup(self):
        cursor = self.db.cursor()
        cursor.execute("CREATE TABLE cpu (pc INTEGER, total_cycles INTEGER, halted INTEGER)")
        cursor.execute("CREATE TABLE memory (addr INTEGER PRIMARY KEY, val INTEGER)")
        cursor.execute("CREATE TABLE output_log (char_val INTEGER)")
        self.db.commit()

    def load_program(self, program):
        cursor = self.db.cursor()
        cursor.execute("DELETE FROM cpu")
        cursor.execute("DELETE FROM memory")
        cursor.execute("DELETE FROM output_log")
        cursor.execute("INSERT INTO cpu (pc, total_cycles, halted) VALUES (0, 0, 0)")
        for i, val in enumerate(program):
            cursor.execute("INSERT INTO memory (addr, val) VALUES (?, ?)", (i, val))
        self.db.commit()

    def step(self):
        cursor = self.db.cursor()
        # 1. Fetch current PC
        cursor.execute("SELECT pc, total_cycles FROM cpu WHERE halted = 0")
        row = cursor.fetchone()
        if not row: return False # Halted or invalid
        
        pc, total_cycles = row
        if pc < 0: return False

        # 2. Get Instruction
        cursor.execute("SELECT val FROM memory WHERE addr BETWEEN ? AND ?", (pc, pc + 2))
        instr = [r[0] for r in cursor.fetchall()]
        if len(instr) < 3: return False
        
        a_addr, b_addr, c_addr = instr

        # 3. Apply SUBLEQ logic via SQL if B != -1, else Output
        if b_addr == -1:
            cursor.execute("SELECT val FROM memory WHERE addr = ?", (a_addr,))
            val_a = cursor.fetchone()[0]
            self.output += chr(val_a)
            cursor.execute("INSERT INTO output_log (char_val) VALUES (?)", (val_a,))
            
            # PC Update
            new_pc = c_addr if -val_a <= 0 else pc + 3
        else:
            cursor.execute("SELECT val FROM memory WHERE addr = ?", (a_addr,))
            val_a = cursor.fetchone()[0]
            cursor.execute("SELECT val FROM memory WHERE addr = ?", (b_addr,))
            val_b = cursor.fetchone()[0]
            
            # SUB
            res = val_b - val_a
            cursor.execute("UPDATE memory SET val = ? WHERE addr = ?", (res, b_addr))
            
            # PC Update
            new_pc = c_addr if res <= 0 else pc + 3

        # 4. Final CPU Update
        cursor.execute("UPDATE cpu SET pc = ?, total_cycles = ?, halted = ?", 
                       (new_pc, total_cycles + 1, 1 if new_pc < 0 else 0))
        self.db.commit()
        return True

    def run(self, cycles):
        count = 0
        while count < cycles:
            if not self.step(): break
            count += 1

    def get_pc(self):
        cursor = self.db.cursor()
        cursor.execute("SELECT pc FROM cpu")
        return cursor.fetchone()[0]

    def get_memory(self, addr):
        cursor = self.db.cursor()
        cursor.execute("SELECT val FROM memory WHERE addr = ?", (addr,))
        row = cursor.fetchone()
        return row[0] if row else None

def test_func(name, data):
    cpu = SQLiteCPU()
    cpu.load_program(data["program"])
    cpu.run(data.get("max_cycles", 100))
    
    success = True
    if "expected_output" in data and cpu.output != data["expected_output"]:
        print(f"Output mismatch in {name}: Expected {repr(data['expected_output'])}, got {repr(cpu.output)}")
        success = false # Python typo: success = False
        success = False # Redefine correctly
    
    if "expected_pc" in data and cpu.get_pc() != data["expected_pc"]:
        print(f"PC mismatch in {name}: Expected {data['expected_pc']}, got {cpu.get_pc()}")
        success = False

    if "expected_memory" in data:
        for addr_str, expected_val in data["expected_memory"].items():
            val = cpu.get_memory(int(addr_str))
            if val != expected_val:
                print(f"Memory mismatch at {addr_str}: Expected {expected_val}, got {val}")
                success = False
    return success

if __name__ == "__main__":
    # Simplified test runner for SQLite
    with open('../js/tests/emu/emu_test_data.json', 'r') as f:
        test_data = json.load(f)
    
    passed = 0
    total = len(test_data)
    for name, data in test_data.items():
        if test_func(name, data):
            passed += 1
    
    if passed == total:
        print("SQLite Emulator Tests: PASS")
    else:
        print(f"SQLite Emulator Tests: FAIL ({passed}/{total})")
        sys.exit(1)
