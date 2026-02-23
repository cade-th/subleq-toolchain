return {
	label_resolution = {
		input = "START: 0 0 START",
		expected = { 0, 0, 0 }
	},
	macro_resolution = {
		input = [[
			#define CLEAR(target) target target 3 endm
			START: CLEAR(X)
			0 0 -1
			X: 10
		]],
		expected = { 6, 6, 3, 0, 0, -1, 10 }
	}
}
