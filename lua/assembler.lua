local lexer = require("lexer")

local Assembler = {}

-- Helper to parse macro arguments in a definition
local function parse_def_args(tokens, pos)
	local args = {}
	if tokens[pos] and tokens[pos].type == "LPAREN" then
		pos = pos + 1
		while tokens[pos] and tokens[pos].type ~= "RPAREN" do
			if tokens[pos].type == "SYMBOL" then
				table.insert(args, tokens[pos].value)
			end
			pos = pos + 1
			if tokens[pos] and tokens[pos].type == "COMMA" then
				pos = pos + 1
			end
		end
		if tokens[pos] and tokens[pos].type == "RPAREN" then
			pos = pos + 1
		end
	end
	return args, pos
end

-- Helper to collect arguments in a macro call
local function collect_call_args(tokens, pos, argc)
	local args = {}
	if tokens[pos] and tokens[pos].type == "LPAREN" then
		pos = pos + 1
		for a = 1, argc do
			local arg_tokens = {}
			while tokens[pos] and tokens[pos].type ~= "COMMA" and tokens[pos].type ~= "RPAREN" do
				table.insert(arg_tokens, tokens[pos])
				pos = pos + 1
			end
			table.insert(args, arg_tokens)
			if tokens[pos] and tokens[pos].type == "COMMA" then
				pos = pos + 1
			end
		end
		if tokens[pos] and tokens[pos].type == "RPAREN" then
			pos = pos + 1
		end
		return args, pos
	end
	return nil, pos
end

-- Substitute parameters in macro body
local function substitute(body, params, args)
	local result = {}
	for _, body_token in ipairs(body) do
		local substituted = false
		if body_token.type == "SYMBOL" then
			for idx, param_name in ipairs(params) do
				if body_token.value == param_name then
					for _, arg_token in ipairs(args[idx] or {}) do
						table.insert(result, arg_token)
					end
					substituted = true
					break
				end
			end
		end
		if not substituted then
			table.insert(result, body_token)
		end
	end
	return result
end

-- Pass 1: Recursive expansion and discovery
local function expand(tokens, macros, symbols, output, pc)
	local i = 1
	while i <= #tokens do
		local t = tokens[i]

		if t.type == "HASH" and tokens[i + 1] and tokens[i + 1].value == "define" then
			-- Macro Definition
			local name = tokens[i + 2].value
			local params, next_pos = parse_def_args(tokens, i + 3)
			local body = {}
			while next_pos <= #tokens do
				local bt = tokens[next_pos]
				if bt.type == "SYMBOL" and bt.value:lower() == "endm" then
					next_pos = next_pos + 1
					break
				end
				table.insert(body, bt)
				next_pos = next_pos + 1
			end
			macros[name] = { params = params, body = body, argc = #params }
			i = next_pos
		elseif t.type == "LABEL" then
			-- Label Discovery
			symbols[t.value] = pc
			i = i + 1
		elseif t.type == "SYMBOL" and macros[t.value] then
			-- Macro Call
			local macro = macros[t.value]
			local args, next_pos = collect_call_args(tokens, i + 1, macro.argc)
			if args or macro.argc == 0 then
				local substituted = substitute(macro.body, macro.params, args or {})
				pc = expand(substituted, macros, symbols, output, pc)
				i = next_pos
			else
				-- Not a macro call after all (missing parens for argc > 0)
				table.insert(output, t)
				pc = pc + 1
				i = i + 1
			end
		elseif t.type == "NUMBER" or t.type == "SYMBOL" then
			-- Value (Number or Label reference)
			table.insert(output, t)
			pc = pc + 1
			i = i + 1
		else
			-- Other tokens (like commas or parens outside macros) skip
			i = i + 1
		end
	end
	return pc
end

function Assembler.pass1(source)
	local tokens = (type(source) == "string") and lexer(source) or source
	local macros, symbols, expanded = {}, {}, {}
	local pc = expand(tokens, macros, symbols, expanded, 0)
	return {
		macros = macros,
		symbols = symbols,
		tokens = expanded,
		pc = pc
	}
end

function Assembler.assemble(source)
	local res = Assembler.pass1(source)

	-- Pass 2: Resolution
	local binary = {}
	for _, t in ipairs(res.tokens) do
		if t.type == "SYMBOL" then
			local addr = res.symbols[t.value]
			if not addr then error("Undefined label: " .. t.value) end
			table.insert(binary, addr)
		elseif t.type == "NUMBER" then
			table.insert(binary, tonumber(t.value))
		end
	end

	return binary
end

return {
	assemble = Assembler.assemble,
	pass1 = Assembler.pass1
}
