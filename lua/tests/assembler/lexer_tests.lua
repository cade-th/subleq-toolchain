local tokenize = require("lexer")
local tester = require("tests.tester")
local test_data = require("tests.assembler.lexer.lexer_data")

local lexer_test = function(test_name, data)
	local output = tokenize(data.input)
	for i, token in ipairs(output) do
		if not data.expected[i] then
			print(string.format("Extra token at index %d in %s: Got %s:%s",
				i, test_name, token.type, token.value))
			return false
		end
		if token.type ~= data.expected[i].type or token.value ~= data.expected[i].value then
			print(string.format("Mismatch at index %d in %s: Expected %s:%s, got %s:%s",
				i, test_name, data.expected[i].type, data.expected[i].value, token.type, token.value))
			return false
		end
	end
	if #output < #data.expected then
		print(string.format("Missing tokens in %s: Expected %d, got %d",
			test_name, #data.expected, #output))
		return false
	end
	return true
end

tester.run("Lexer Tests", test_data, lexer_test)
