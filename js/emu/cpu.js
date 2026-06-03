const cpu = {
    pc: 0,
    memory: [],
    total_cycles: 0,

    subleq: function () {
        if (this.pc < 0) return 0;

        const addr_A = this.memory[this.pc];
        const addr_B = this.memory[this.pc + 1];
        const addr_C = this.memory[this.pc + 2];

        // Mechanism for printing a character to the screen
        if (addr_B === -1) {
            const val_a = this.memory[addr_A];
            process.stdout.write(String.fromCharCode(val_a));

            // this is to help check for the end of a string via a negative value
            if (-val_a <= 0) {
                this.pc = addr_C;
            } else {
                this.pc += 3;
            }
        } else {
            const val_a = this.memory[addr_A];
            const val_b = this.memory[addr_B];
            const result = val_b - val_a;
            this.memory[addr_B] = result;

            if (result <= 0) {
                this.pc = addr_C;
            } else {
                this.pc += 3;
            }
        }

        return 1;
    },

    step: function () {
        this.total_cycles += this.subleq();
    },

    run: function (cycles) {
        while (this.total_cycles < cycles && this.pc >= 0) {
            this.step();
        }
    },

    load_program: function (input) {
        // Enforce that memory is just the input array
        this.memory = Array.isArray(input) ? input : [];
        this.pc = 0;
        this.total_cycles = 0;
    },

    reset: function () {
        this.pc = 0;
        this.total_cycles = 0;
    }
};

module.exports = cpu;
