local tester = {}

--- Runs a test suite.
-- @param suite_name Name of the test suite (e.g. "Lexer")
-- @param test_data Table containing named test cases
-- @param test_func Function(name, data) that returns true/false
tester.run = function(suite_name, test_data, test_func)
	local success = true
	local count = 0

	for name, data in pairs(test_data) do
		count = count + 1
		local ok, err = pcall(test_func, name, data)
		if not ok then
			print(string.format("Error in test %s: %s", name, tostring(err)))
			success = false
		elseif not err then -- test_func returns false for failure
			success = false
		end
	end

	if count == 0 then
		print(string.format("%s: NO TESTS FOUND", suite_name))
		return false
	end

	if success then
		print(string.format("%s: PASS", suite_name))
	else
		print(string.format("%s: FAIL", suite_name))
		os.exit(1)
	end

	return success
end

return tester
