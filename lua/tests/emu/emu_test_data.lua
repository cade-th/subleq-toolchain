return {
	halt = {
		program = { 0, 0, -1 },
		expected_output = "",
		expected_pc = -1,
		max_cycles = 1
	},
	hello_a = {
		program = { 3, -1, -1, 65 },
		expected_output = "A",
		expected_pc = -1,
		max_cycles = 1
	},
	hello_hi = {
		program = {
			6, -1, 3,
			7, -1, -1,
			72,
			73
		},
		expected_output = "HI",
		expected_pc = -1,
		max_cycles = 2
	}
}
