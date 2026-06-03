import sys

class CPU:
    def __init__(self):
        self.pc = 0
        self.memory = []
        self.total_cycles = 0

    def subleq(self):
        if self.pc < 0:
            return 0

        addr_A = self.memory[self.pc]
        addr_B = self.memory[self.pc + 1]
        addr_C = self.memory[self.pc + 2]

        # Mechanism for printing a character to the screen
        if addr_B == -1:
            val_a = self.memory[addr_A]
            sys.stdout.write(chr(val_a))

            # this is to help check for the end of a string via a negative value
            if -val_a <= 0:
                self.pc = addr_C
            else:
                self.pc += 3
        else:
            val_a = self.memory[addr_A]
            val_b = self.memory[addr_B]
            result = val_b - val_a
            self.memory[addr_B] = result

            if result <= 0:
                self.pc = addr_C
            else:
                self.pc += 3

        return 1

    def step(self):
        self.total_cycles += self.subleq()

    def run(self, cycles):
        while self.total_cycles < cycles and self.pc >= 0:
            self.step()

    def load_program(self, input_program):
        self.memory = list(input_program)
        self.pc = 0
        self.total_cycles = 0

    def reset(self):
        self.pc = 0
        self.total_cycles = 0
