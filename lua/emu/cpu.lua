local cpu
cpu = {
	pc = 0,
	actual_memory = {},
	total_cycles = 0,

	-- Memory proxy allows 0-based access to the 1-based lua table
	memory = setmetatable({}, {
		__index = function(_, k)
			return cpu.actual_memory[k + 1]
		end,
		__newindex = function(_, k, v)
			cpu.actual_memory[k + 1] = v
		end
	}),

	subleq = function()
		if (cpu.pc < 0) then return 0 end

		local addr_A = cpu.memory[cpu.pc]
		local addr_B = cpu.memory[cpu.pc + 1]
		local addr_C = cpu.memory[cpu.pc + 2]

		-- mechanism for printing a character to the screen
		if (addr_B == -1) then
			local val_a = cpu.memory[addr_A]
			io.write(string.char(val_a))

			-- this is to help check for the end of a string via a negative value
			if (-val_a <= 0) then
				cpu.pc = addr_C
			else
				cpu.pc = cpu.pc + 3
			end
		else
			local val_a = cpu.memory[addr_A]
			local val_b = cpu.memory[addr_B]
			local result = val_b - val_a
			cpu.memory[addr_B] = result

			if (result <= 0) then
				cpu.pc = addr_C
			else
				cpu.pc = cpu.pc + 3
			end
		end

		return 1
	end,

	step = function()
		cpu.total_cycles = cpu.total_cycles + cpu.subleq()
	end,

	run = function(cycles)
		while (cpu.total_cycles < cycles and cpu.pc >= 0) do
			cpu.step()
		end
	end,

	load_program = function(input)
		cpu.actual_memory = input
		cpu.pc = 0
		cpu.total_cycles = 0
	end,

	reset = function()
		cpu.pc = 0
		cpu.total_cycles = 0
	end,
}
return cpu
