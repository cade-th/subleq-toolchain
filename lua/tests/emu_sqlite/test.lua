local json = require("json")
local SQLiteCPU = require("sqlite_cpu")

local function run_test(name, data)
    local cpu = SQLiteCPU.new()
    cpu:load_program(data.program)
    cpu:run(data.max_cycles or 100)

    local success = true

    if data.expected_output and cpu.output ~= data.expected_output then
        print(string.format("Output mismatch in %s: Expected %q, got %q", name, data.expected_output, cpu.output))
        success = false
    end

    if data.expected_pc and cpu:get_pc() ~= data.expected_pc then
        print(string.format("PC mismatch in %s: Expected %d, got %d", name, data.expected_pc, cpu:get_pc()))
        success = false
    end

    if data.expected_memory then
        for addr_str, expected_val in pairs(data.expected_memory) do
            local addr = tonumber(addr_str)
            local actual = cpu:get_memory(addr)
            if actual ~= expected_val then
                print(string.format("Memory mismatch in %s at %d: Expected %d, got %d", name, addr, expected_val, actual))
                success = false
            end
        end
    end

    cpu:cleanup()
    return success
end

local function main()
    local f = io.open("emu_test_data.json", "r")
    if not f then error("Could not open test data") end
    local content = f:read("*all")
    f:close()

    local test_data = json.decode(content)
    local total = 0
    local passed = 0

    for name, data in pairs(test_data) do
        total = total + 1
        if run_test(name, data) then
            passed = passed + 1
        end
    end

    if passed == total then
        print("SQLite Emulator Tests (Lua): PASS")
    else
        print(string.format("SQLite Emulator Tests (Lua): FAIL (%d/%d)", passed, total))
        os.exit(1)
    end
end

main()
