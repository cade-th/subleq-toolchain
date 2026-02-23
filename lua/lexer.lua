local token_definitions = {
	{ "NUMBER",     "^(%-?%d+%.?%d*)" },
	{ "HASH",       "^(#)" },
	{ "LABEL",      "^([%a_][%w_]*):" },
	{ "SYMBOL",     "^([%a_][%w_]*)" },
	{ "LPAREN",     "^(%()" },
	{ "RPAREN",     "^(%))" },
	{ "COMMA",      "^(,)" },
	{ "WHITESPACE", "^(%s+)" },
}

local tokenize = function(input)
	local tokens = {}
	local pos = 1

	while pos <= #input do
		local matched = false
		local substring = input:sub(pos)

		for _, def in ipairs(token_definitions) do
			local type, pattern = def[1], def[2]
			local start, finish, capture = substring:find(pattern)

			if start then
				if type ~= "WHITESPACE" then
					table.insert(tokens,
						{ type = type, value = capture or substring:sub(start, finish) })
				end

				pos = pos + finish
				matched = true
				break
			end
		end

		if not matched then
			error("Unexpected character at position " .. pos .. ": " .. input:sub(pos, pos))
		end
	end

	return tokens
end

return tokenize
