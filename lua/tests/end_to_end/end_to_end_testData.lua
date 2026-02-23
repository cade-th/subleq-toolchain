return {
	simple_sub = {
		-- Assembly:
		-- 6 7 3 
		-- 0 0 -1
		-- 5 (X at addr 6)
		-- 10 (Y at addr 7)
		input = "6 7 3 0 0 -1 5 10",
		expected_memory = {
			[7] = 5 -- address 7 (Y)
		},
		expected_pc = -1
	},
	loop_test = {
		input = " \
			START: \
				ONE Y HALT \
				Z Z START \
			HALT: \
				Z Z -1 \
			Y: 3 \
			ONE: 1 \
			Z: 0 \
		",
		expected_memory = {
			[9] = 0 -- Y (address 9)
		},
		expected_pc = -1
	},
	macro_hi = {
		input = [[
			#define printH()
				9 -1 3
			endm
			#define printI()
				10 -1 6
			endm

			printH()
			printI()
			0 0 -1

			H: 72
			I: 73
		]],
		expected_output = "HI",
		expected_pc = -1
	},
	macro_args = {
		input = [[
			#define clear(target, next_addr)
				target target next_addr
			endm

			START:
				clear(Y, 3)
				clear(X, 6)
				0 0 -1
			Y: 10
			X: 20
		]],
		expected_memory = {
			[9] = 0, -- Y (addr 9)
			[10] = 0  -- X (addr 10)
		},
		expected_pc = -1
	}
}
