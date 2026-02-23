local assembler = require("assembler")
local tester = require("tests.tester")
local pass1_data = require("tests.assembler.assembler.pass1_data")
local pass2_data = require("tests.assembler.assembler.pass2_data")
local steps_data = require("tests.assembler.assembler.steps_data")

local test_pass1 = function(name, data)
	local res = assembler.pass1(data.input)

	-- Check PC
	if data.expected.pc and res.pc ~= data.expected.pc then
		print(string.format("PC mismatch in %s: Expected %d, got %d", name, data.expected.pc, res.pc))
		return false
	end

	-- Check Symbols
	if data.expected.symbols then
		for label, addr in pairs(data.expected.symbols) do
			if res.symbols[label] ~= addr then
				print(string.format("Symbol mismatch in %s: Label %s expected at %d, got %s",
					name, label, addr, tostring(res.symbols[label])))
				return false
			end
		end
	end

	-- Check Macros (just check existence and argc for simplicity)
	if data.expected.macros then
		for name_m, expected_m in pairs(data.expected.macros) do
			local actual = res.macros[name_m]
			if not actual or actual.argc ~= expected_m.argc then
				print(string.format("Macro mismatch in %s: Macro %s", name, name_m))
				return false
			end
		end
	end

	-- Check Tokens
	if data.expected.tokens then
		if #res.tokens ~= #data.expected.tokens then
			print(string.format("Token count mismatch in %s: Expected %d, got %d",
				name, #data.expected.tokens, #res.tokens))
			return false
		end
		for i, tok in ipairs(res.tokens) do
			if tok.type ~= data.expected.tokens[i].type or tok.value ~= data.expected.tokens[i].value then
				print(string.format("Token mismatch in %s at index %d: Expected %s:%s, got %s:%s",
					name, i, data.expected.tokens[i].type, data.expected.tokens[i].value, tok.type, tok.value))
				return false
			end
		end
	end

	return true
end

local test_pass2 = function(name, data)
	local binary = assembler.assemble(data.input)

	if #binary ~= #data.expected then
		print(string.format("Binary size mismatch in %s: Expected %d, got %d",
			name, #data.expected, #binary))
		return false
	end

	for i, val in ipairs(binary) do
		if val ~= data.expected[i] then
			print(string.format("Binary mismatch in %s at index %d: Expected %d, got %d",
				name, i, data.expected[i], val))
			return false
		end
	end

	return true
end

local test_integrated_step = function(name, data)
	-- Wrap data to match test_pass1 expectations
	local pass1_data_wrapper = {
		input = data.input,
		expected = data.pass1
	}
	if not test_pass1(name .. " (Pass 1)", pass1_data_wrapper) then
		return false
	end

	-- Wrap data to match test_pass2 expectations
	local pass2_data_wrapper = {
		input = data.input,
		expected = data.binary
	}
	if not test_pass2(name .. " (Pass 2)", pass2_data_wrapper) then
		return false
	end

	return true
end

tester.run("Assembler Pass 1 Tests", pass1_data, test_pass1)
tester.run("Assembler Pass 2 Tests", pass2_data, test_pass2)
tester.run("Assembler Evolutionary Steps", steps_data, test_integrated_step)
