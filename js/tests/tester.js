const tester = {
    run: (suiteName, testData, testFunc) => {
        let success = true;
        let count = 0;

        for (const name in testData) {
            count++;
            try {
                const ok = testFunc(name, testData[name]);
                if (!ok) {
                    success = false;
                }
            } catch (err) {
                console.error(`Error in test ${name}: ${err}`);
                success = false;
            }
        }

        if (count === 0) {
            console.log(`${suiteName}: NO TESTS FOUND`);
            return false;
        }

        if (success) {
            console.log(`${suiteName}: PASS`);
        } else {
            console.log(`${suiteName}: FAIL`);
            process.exit(1);
        }

        return success;
    }
};

module.exports = tester;
