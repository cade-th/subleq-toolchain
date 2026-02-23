local cpu = require("emu.cpu")
local tester = require("tests.tester")
local test_data = require("tests.emu.emu_test_data")

local emu_test = function(test_name, data)
	cpu.load_program(data.program)
	
	-- Mock io.write
	local output_buffer = ""
	local original_io_write = io.write
	io.write = function(str)
		output_buffer = output_buffer .. str
	end

	-- Safety cycle limit
	local cycles = data.max_cycles or 100
	cpu.run(cycles)

	-- Restore io.write
	io.write = original_io_write

	local success = true

	-- Check Output
	if data.expected_output and output_buffer ~= data.expected_output then
		print(string.format("Output mismatch in %s: Expected %q, got %q",
			test_name, data.expected_output, output_buffer))
		success = false
	end

	-- Check PC
	if data.expected_pc and cpu.pc ~= data.expected_pc then
		print(string.format("PC mismatch in %s: Expected %d, got %d",
			test_name, data.expected_pc, cpu.pc))
		success = false
	end

	-- Check Memory
	if data.expected_memory then
		-- addr in expected_memory is 0-based
		for addr, expected_val in pairs(data.expected_memory) do
			if cpu.memory[addr] ~= expected_val then
				print(string.format("Memory mismatch at address %d in %s: Expected %d, got %s",
					addr, test_name, expected_val, tostring(cpu.memory[addr])))
				success = false
			end
		end
	end

	return success
end

tester.run("Emulator Tests", test_data, emu_test)
