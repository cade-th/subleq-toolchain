-- Subleq Emulator in SQLite
-- This script sets up the CPU state and provides a recursive step function.

-- 1. Setup Tables
CREATE TABLE IF NOT EXISTS cpu (
    pc INTEGER DEFAULT 0,
    total_cycles INTEGER DEFAULT 0,
    halted INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS memory (
    addr INTEGER PRIMARY KEY,
    val INTEGER
);

-- 2. Procedure to load a program
-- Usage: DELETE FROM memory; INSERT INTO memory (addr, val) VALUES (0, val0), (1, val1), ...;
--        DELETE FROM cpu; INSERT INTO cpu (pc, total_cycles, halted) VALUES (0, 0, 0);

-- 3. The Instruction Step (One Step)
-- We use a CTE or a set of UPDATEs to simulate one 'subleq' instruction.
-- Since SQLite doesn't have native looping without an external driver or recursive CTEs,
-- we'll define the logic to perform exactly ONE step.

CREATE VIEW IF NOT EXISTS step_logic AS
WITH current_state AS (
    SELECT pc, total_cycles FROM cpu WHERE halted = 0
),
instruction AS (
    SELECT 
        c.pc,
        m1.val AS a_addr,
        m2.val AS b_addr,
        m3.val AS c_addr
    FROM current_state c
    JOIN memory m1 ON m1.addr = c.pc
    JOIN memory m2 ON m2.addr = c.pc + 1
    JOIN memory m3 ON m3.addr = c.pc + 2
),
operands AS (
    SELECT 
        i.*,
        ma.val AS val_a,
        mb.val AS val_b
    FROM instruction i
    LEFT JOIN memory ma ON ma.addr = i.a_addr
    LEFT JOIN memory mb ON mb.addr = i.b_addr
)
SELECT 
    pc,
    a_addr,
    b_addr,
    c_addr,
    val_a,
    val_b,
    (val_b - val_a) AS result,
    CASE 
        WHEN b_addr = -1 THEN 'OUT'
        ELSE 'SUB'
    END AS op_type
FROM operands;

-- 4. Trigger-like operation to actually apply the step
-- To be run by the user: 
-- INSERT INTO cpu_update SELECT ... FROM step_logic;

CREATE TABLE IF NOT EXISTS output_log (
    char_val INTEGER
);

-- We'll use a script to run the steps.
