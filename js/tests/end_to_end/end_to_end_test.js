const assembler = require('../../assembler');
const cpu = require('../../emu/cpu');
const tester = require('../tester');
const testData = require('./end_to_end_data.json');

const e2eTest = (name, data) => {
    // Pipeline
    const program = assembler.assemble(data.input);
    cpu.load_program(program);

    let outputBuffer = "";
    const originalStdoutWrite = process.stdout.write;
    process.stdout.write = (str) => {
        outputBuffer += str;
        return true;
    };

    cpu.run(data.max_cycles || 1000);
    process.stdout.write = originalStdoutWrite;

    let success = true;

    // Check Output
    if (data.expected_output !== undefined && outputBuffer !== data.expected_output) {
        console.log(`Output mismatch in ${name}: Expected ${JSON.stringify(data.expected_output)}, got ${JSON.stringify(outputBuffer)}`);
        success = false;
    }

    // Check Memory
    if (data.expected_memory) {
        for (const addr in data.expected_memory) {
            const expectedVal = data.expected_memory[addr];
            if (cpu.memory[addr] !== expectedVal) {
                console.log(`Memory mismatch in ${name} at address ${addr}: Expected ${expectedVal}, got ${cpu.memory[addr]}`);
                success = false;
            }
        }
    }

    // Check PC
    if (data.expected_pc !== undefined && cpu.pc !== data.expected_pc) {
        console.log(`PC mismatch in ${name}: Expected ${data.expected_pc}, got ${cpu.pc}`);
        success = false;
    }

    return success;
};

tester.run("End-to-End Tests", testData, e2eTest);
