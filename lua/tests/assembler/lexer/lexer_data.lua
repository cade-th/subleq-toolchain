return {
	one_instruction = {
		input = "0 0 -1",
		expected = {
			{ type = "NUMBER", value = "0" },
			{ type = "NUMBER", value = "0" },
			{ type = "NUMBER", value = "-1" },
		}
	},
	two_instructions = {
		input = " \
	0 0 4             \
	0 0 -1            \
",
		expected = {
			{ type = "NUMBER", value = "0" },
			{ type = "NUMBER", value = "0" },
			{ type = "NUMBER", value = "4" },
			{ type = "NUMBER", value = "0" },
			{ type = "NUMBER", value = "0" },
			{ type = "NUMBER", value = "-1" },
		}
	},
	one_label = {
		input = "    \
	START:         \
		0 0 -1 \
",
		expected = {
			{ type = "LABEL",  value = "START" },
			{ type = "NUMBER", value = "0" },
			{ type = "NUMBER", value = "0" },
			{ type = "NUMBER", value = "-1" },
		}
	},
	two_labels = {
		input = "    \
	START:         \
		0 0 -1 \
		       \
	LOOP:          \
		0 0 0  \
",
		expected = {
			{ type = "LABEL",  value = "START" },
			{ type = "NUMBER", value = "0" },
			{ type = "NUMBER", value = "0" },
			{ type = "NUMBER", value = "-1" },
			{ type = "LABEL",  value = "LOOP" },
			{ type = "NUMBER", value = "0" },
			{ type = "NUMBER", value = "0" },
			{ type = "NUMBER", value = "0" },
		}
	},
	one_macro = {
		input = "    \
	#define VAR 0  \
	START:         \
		0 VAR -1 \
",
		expected = {
			{ type = "HASH",   value = "#" },
			{ type = "SYMBOL", value = "define" },
			{ type = "SYMBOL", value = "VAR" },
			{ type = "NUMBER", value = "0" },
			{ type = "LABEL",  value = "START" },
			{ type = "NUMBER", value = "0" },
			{ type = "SYMBOL", value = "VAR" },
			{ type = "NUMBER", value = "-1" },
		}
	},
	two_macros = {
		input = "       \
	#define VAR1 0    \
	#define VAR2 7    \
	START:            \
		0 VAR1 3  \
		0 VAR2 -1 \
",
		expected = {
			{ type = "HASH",   value = "#" },
			{ type = "SYMBOL", value = "define" },
			{ type = "SYMBOL", value = "VAR1" },
			{ type = "NUMBER", value = "0" },
			{ type = "HASH",   value = "#" },
			{ type = "SYMBOL", value = "define" },
			{ type = "SYMBOL", value = "VAR2" },
			{ type = "NUMBER", value = "7" },
			{ type = "LABEL",  value = "START" },
			{ type = "NUMBER", value = "0" },
			{ type = "SYMBOL", value = "VAR1" },
			{ type = "NUMBER", value = "3" },
			{ type = "NUMBER", value = "0" },
			{ type = "SYMBOL", value = "VAR2" },
			{ type = "NUMBER", value = "-1" },
		}
	}
}
