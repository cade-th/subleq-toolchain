local assembler = require("assembler")

local function main()
    local source = io.read("*all")
    if not source or source == "" then
        return
    end

    local status, program = pcall(assembler.assemble, source)
    if not status then
        io.stderr:write("Assembly failed: " .. tostring(program) .. "\n")
        os.exit(1)
    end

    for _, val in ipairs(program) do
        -- Convert to unsigned 32-bit hex
        local uval = val
        if uval < 0 then
            uval = 0x100000000 + uval
        end
        print(string.format("%08X", uval))
    end
end

main()
