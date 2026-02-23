return {
	simple_labels = {
		input = "START: 0 0 -1",
		expected = {
			pc = 3,
			symbols = { START = 0 },
			tokens = {
				{ type = "NUMBER", value = "0" },
				{ type = "NUMBER", value = "0" },
				{ type = "NUMBER", value = "-1" }
			}
		}
	},
	macro_discovery = {
		input = [[
			#define HALT() 0 0 -1 endm
			START: HALT() 5
		]],
		expected = {
			pc = 4,
			symbols = { START = 0 },
			macros = { HALT = { argc = 0 } },
			tokens = {
				{ type = "NUMBER", value = "0" },
				{ type = "NUMBER", value = "0" },
				{ type = "NUMBER", value = "-1" },
				{ type = "NUMBER", value = "5" }
			}
		}
	}
}
