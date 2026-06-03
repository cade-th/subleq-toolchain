const tokenize = require('../../lexer');
const tester = require('../tester');
const testData = require('./lexer_data.json');

const lexerTest = (testName, data) => {
    const output = tokenize(data.input);
    for (let i = 0; i < output.length; i++) {
        const token = output[i];
        if (!data.expected[i]) {
            console.log(`Extra token at index ${i} in ${testName}: Got ${token.type}:${token.value}`);
            return false;
        }
        if (token.type !== data.expected[i].type || token.value !== data.expected[i].value) {
            console.log(`Mismatch at index ${i} in ${testName}: Expected ${data.expected[i].type}:${data.expected[i].value}, got ${token.type}:${token.value}`);
            return false;
        }
    }
    if (output.length < data.expected.length) {
        console.log(`Missing tokens in ${testName}: Expected ${data.expected.length}, got ${output.length}`);
        return false;
    }
    return true;
};

tester.run("Lexer Tests", testData, lexerTest);
