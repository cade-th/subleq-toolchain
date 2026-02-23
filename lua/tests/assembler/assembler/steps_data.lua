return {
	step1_identity = {
		input = "10 -5 100",
		pass1 = {
			pc = 3,
			symbols = {},
			macros = {}
		},
		binary = { 10, -5, 100 }
	},
	step2_backward_labels = {
		input = "START: 0 0 START",
		pass1 = {
			pc = 3,
			symbols = { START = 0 },
			macros = {}
		},
		binary = { 0, 0, 0 }
	},
	step3_forward_labels = {
		input = "0 0 TARGET TARGET: 10",
		pass1 = {
			pc = 4,
			symbols = { TARGET = 3 },
			macros = {}
		},
		binary = { 0, 0, 3, 10 }
	},
	step5_simple_macro = {
		input = [[
			#define CLEAR_Z() 5 5 2 endm
			CLEAR_Z()
		]],
		pass1 = {
			pc = 3,
			symbols = {},
			macros = { CLEAR_Z = { argc = 0 } }
		},
		binary = { 5, 5, 2 }
	},
	step6_nested_macros = {
		input = [[
			#define VAL() 42 endm
			#define WRAPPER() VAL() 0 -1 endm
			WRAPPER()
		]],
		pass1 = {
			pc = 3,
			symbols = {},
			macros = { VAL = { argc = 0 }, WRAPPER = { argc = 0 } }
		},
		binary = { 42, 0, -1 }
	},
	step7_macro_args = {
		input = [[
			#define MOVE(a, b) a b 0 endm
			MOVE(10, 20)
		]],
		pass1 = {
			pc = 3,
			symbols = {},
			macros = { MOVE = { argc = 2 } }
		},
		binary = { 10, 20, 0 }
	}
}
