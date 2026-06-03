local SQLiteCPU = {}
SQLiteCPU.__index = SQLiteCPU

function SQLiteCPU.new()
    local self = setmetatable({}, SQLiteCPU)
    self.db_name = "subleq_test.db"
    os.remove(self.db_name)
    self.output = ""
    self:init_db()
    return self
end

function SQLiteCPU:exec(sql)
    local cmd = string.format("sqlite3 %s %q", self.db_name, sql)
    local f = io.popen(cmd)
    local res = f:read("*all")
    f:close()
    return res
end

function SQLiteCPU:cleanup()
    os.remove(self.db_name)
end

function SQLiteCPU:init_db()
    self:exec([[
        CREATE TABLE cpu (pc INTEGER, total_cycles INTEGER, halted INTEGER);
        CREATE TABLE memory (addr INTEGER PRIMARY KEY, val INTEGER);
        INSERT INTO cpu (pc, total_cycles, halted) VALUES (0, 0, 0);
    ]])
end

function SQLiteCPU:load_program(program)
    self:exec("DELETE FROM cpu; DELETE FROM memory; INSERT INTO cpu (pc, total_cycles, halted) VALUES (0, 0, 0);")
    local vals = {}
    for i, v in ipairs(program) do
        table.insert(vals, string.format("(%d, %d)", i - 1, v))
    end
    if #vals > 0 then
        self:exec("INSERT INTO memory (addr, val) VALUES " .. table.concat(vals, ","))
    end
    self.output = ""
end

function SQLiteCPU:step()
    local res = self:exec("SELECT pc, halted FROM cpu")
    local pc, halted = res:match("(%-?%d+)|(%d+)")
    pc = tonumber(pc)
    halted = tonumber(halted)

    if not pc or halted == 1 or pc < 0 then return false end

    -- Fetch instruction
    local instr_res = self:exec(string.format("SELECT val FROM memory WHERE addr BETWEEN %d AND %d", pc, pc + 2))
    local instr = {}
    for v in instr_res:gmatch("%-?%d+") do
        table.insert(instr, tonumber(v))
    end

    if #instr < 3 then return false end

    local a_addr, b_addr, c_addr = instr[1], instr[2], instr[3]

    if b_addr == -1 then
        local val_a_res = self:exec(string.format("SELECT val FROM memory WHERE addr = %d", a_addr))
        local val_a = tonumber(val_a_res:match("%-?%d+"))
        if not val_a then return false end

        self.output = self.output .. string.char(val_a)
        local next_pc = (-val_a <= 0) and c_addr or (pc + 3)
        self:exec(string.format("UPDATE cpu SET pc = %d, total_cycles = total_cycles + 1, halted = %d", 
                                next_pc, (next_pc < 0 and 1 or 0)))
    else
        local ops_res = self:exec(string.format("SELECT val FROM memory WHERE addr IN (%d, %d)", a_addr, b_addr))
        local vals = {}
        for v in ops_res:gmatch("%-?%d+") do
            table.insert(vals, tonumber(v))
        end
        -- Note: IN doesn't guarantee order, but let's fetch specifically
        local val_a_res = self:exec(string.format("SELECT val FROM memory WHERE addr = %d", a_addr))
        local val_a = tonumber(val_a_res:match("%-?%d+"))
        local val_b_res = self:exec(string.format("SELECT val FROM memory WHERE addr = %d", b_addr))
        local val_b = tonumber(val_b_res:match("%-?%d+"))

        if not val_a or not val_b then return false end

        local result = val_b - val_a
        self:exec(string.format("UPDATE memory SET val = %d WHERE addr = %d", result, b_addr))
        local next_pc = (result <= 0) and c_addr or (pc + 3)
        self:exec(string.format("UPDATE cpu SET pc = %d, total_cycles = total_cycles + 1, halted = %d", 
                                next_pc, (next_pc < 0 and 1 or 0)))
    end

    return true
end

function SQLiteCPU:run(max_cycles)
    local count = 0
    while count < max_cycles do
        if not self:step() then break end
        count = count + 1
    end
end

function SQLiteCPU:get_pc()
    local res = self:exec("SELECT pc FROM cpu")
    return tonumber(res:match("%-?%d+"))
end

function SQLiteCPU:get_memory(addr)
    local res = self:exec(string.format("SELECT val FROM memory WHERE addr = %d", addr))
    return tonumber(res:match("%-?%d+"))
end

return SQLiteCPU
