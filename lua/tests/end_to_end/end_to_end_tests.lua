local assembler = require("assembler")
local cpu = require("emu.cpu")
local tester = require("tests.tester")
local test_data = require("tests.end_to_end.end_to_end_testData")

local e2e_test = function(name, data)
	-- Unified Pipeline
	local program = assembler.assemble(data.input)

	cpu.load_program(program)

	-- Mock output
	local output_buffer = ""
	local original_io_write = io.write
	io.write = function(str)
		output_buffer = output_buffer .. str
	end

	cpu.run(data.max_cycles or 1000)

	io.write = original_io_write

	local success = true

	-- Check Output
	if data.expected_output and output_buffer ~= data.expected_output then
		print(string.format("Output mismatch in %s: Expected %q, got %q",
			name, data.expected_output, output_buffer))
		success = false
	end

	-- Check Memory
	if data.expected_memory then
		-- addr in expected_memory is 0-based
		for addr, val in pairs(data.expected_memory) do
			if cpu.memory[addr] ~= val then
				print(string.format("Memory mismatch in %s at address %d: Expected %d, got %s",
					name, addr, val, tostring(cpu.memory[addr])))
				success = false
			end
		end
	end

	-- Check PC
	if data.expected_pc ~= nil and cpu.pc ~= data.expected_pc then
		print(string.format("PC mismatch in %s: Expected %d, got %d",
			name, data.expected_pc, cpu.pc))
		success = false
	end

	return success
end

tester.run("End-to-End Tests", test_data, e2e_test)
