const cpu = require('../../emu/cpu');
const tester = require('../tester');
const testData = require('./emu_test_data.json');

const emuTest = (testName, data) => {
    cpu.load_program(data.program);

    let outputBuffer = "";
    const originalStdoutWrite = process.stdout.write;
    process.stdout.write = (str) => {
        outputBuffer += str;
        // Return true to indicate successful write (standard behavior)
        return true;
    };

    // Safety cycle limit
    const cycles = data.max_cycles || 100;
    cpu.run(cycles);

    // Restore stdout.write
    process.stdout.write = originalStdoutWrite;

    let success = true;

    // Check Output
    if (data.expected_output !== undefined && outputBuffer !== data.expected_output) {
        console.log(`Output mismatch in ${testName}: Expected ${JSON.stringify(data.expected_output)}, got ${JSON.stringify(outputBuffer)}`);
        success = false;
    }

    // Check PC
    if (data.expected_pc !== undefined && cpu.pc !== data.expected_pc) {
        console.log(`PC mismatch in ${testName}: Expected ${data.expected_pc}, got ${cpu.pc}`);
        success = false;
    }

    // Check Memory
    if (data.expected_memory) {
        for (const addr in data.expected_memory) {
            const expectedVal = data.expected_memory[addr];
            if (cpu.memory[addr] !== expectedVal) {
                console.log(`Memory mismatch at address ${addr} in ${testName}: Expected ${expectedVal}, got ${cpu.memory[addr]}`);
                success = false;
            }
        }
    }

    return success;
};

tester.run("Emulator Tests", testData, emuTest);
