const assembler = require('../../assembler');
const tester = require('../tester');
const pass1Data = require('./pass1_data.json');
const pass2Data = require('./pass2_data.json');

const pass1_test = (testName, data) => {
    const res = assembler.pass1(data.input);
    let success = true;

    if (res.pc !== data.expected.pc) {
        console.log(`PC mismatch in ${testName}: Expected ${data.expected.pc}, got ${res.pc}`);
        success = false;
    }

    if (data.expected.symbols) {
        for (const label in data.expected.symbols) {
            if (res.symbols[label] !== data.expected.symbols[label]) {
                console.log(`Symbol mismatch for ${label} in ${testName}: Expected ${data.expected.symbols[label]}, got ${res.symbols[label]}`);
                success = false;
            }
        }
    }

    if (data.expected.macros) {
        for (const macroName in data.expected.macros) {
            const macro = res.macros[macroName];
            if (!macro) {
                console.log(`Macro ${macroName} not found in ${testName}`);
                success = false;
            } else if (macro.argc !== data.expected.macros[macroName].argc) {
                console.log(`Macro ${macroName} argc mismatch in ${testName}: Expected ${data.expected.macros[macroName].argc}, got ${macro.argc}`);
                success = false;
            }
        }
    }

    if (data.expected.tokens) {
        if (res.tokens.length !== data.expected.tokens.length) {
            console.log(`Tokens length mismatch in ${testName}: Expected ${data.expected.tokens.length}, got ${res.tokens.length}`);
            success = false;
        } else {
            for (let i = 0; i < res.tokens.length; i++) {
                if (res.tokens[i].type !== data.expected.tokens[i].type || res.tokens[i].value !== data.expected.tokens[i].value) {
                    console.log(`Token mismatch at ${i} in ${testName}: Expected ${data.expected.tokens[i].type}:${data.expected.tokens[i].value}, got ${res.tokens[i].type}:${res.tokens[i].value}`);
                    success = false;
                }
            }
        }
    }

    return success;
};

const pass2_test = (testName, data) => {
    try {
        const binary = assembler.assemble(data.input);
        if (binary.length !== data.expected.length) {
            console.log(`Binary length mismatch in ${testName}: Expected ${data.expected.length}, got ${binary.length}`);
            return false;
        }
        for (let i = 0; i < binary.length; i++) {
            if (binary[i] !== data.expected[i]) {
                console.log(`Binary mismatch at index ${i} in ${testName}: Expected ${data.expected[i]}, got ${binary[i]}`);
                return false;
            }
        }
        return true;
    } catch (e) {
        console.log(`Assembly failed in ${testName}: ${e.message}`);
        return false;
    }
};

tester.run("Assembler Pass 1 Tests", pass1Data, pass1_test);
tester.run("Assembler Pass 2 Tests", pass2Data, pass2_test);
