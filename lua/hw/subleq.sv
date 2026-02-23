module subleq (
    input  logic        clk,
    input  logic        rst_n,

    // Memory interface
    output logic [31:0] mem_addr,
    input  logic [31:0] mem_din,
    output logic [31:0] mem_dout,
    output logic        mem_we,

    // Simple I/O
    output logic [7:0]  out_byte,
    output logic        out_valid,
    
    // Status
    output logic        halted
);

    typedef enum logic [2:0] {
        FETCH_A, FETCH_B, FETCH_C,
        READ_VAL_A, READ_VAL_B,
        EXECUTE,
        HALT
    } state_t;

    state_t state;
    logic [31:0] pc;
    logic [31:0] addr_a, addr_b, addr_c;
    logic [31:0] val_a;
    logic [31:0] sub_result;

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            pc        <= 0;
            state     <= FETCH_A;
            mem_we    <= 0;
            out_valid <= 0;
            halted    <= 0;
        end else begin
            mem_we    <= 0;
            out_valid <= 0;

            case (state)
                FETCH_A: begin
                    mem_addr <= pc;
                    state    <= FETCH_B;
                end

                FETCH_B: begin
                    addr_a   <= mem_din;
                    mem_addr <= pc + 1;
                    state    <= FETCH_C;
                end

                FETCH_C: begin
                    addr_b   <= mem_din;
                    mem_addr <= pc + 2;
                    state    <= READ_VAL_A;
                end

                READ_VAL_A: begin
                    addr_c   <= mem_din;
                    mem_addr <= addr_a;
                    state    <= READ_VAL_B;
                end

                READ_VAL_B: begin
                    val_a    <= mem_din;
                    if (addr_b == 32'hFFFFFFFF) begin
                        // Output mapping
                        out_byte  <= mem_din[7:0];
                        out_valid <= 1;
                        // For output mapping, we jump if val_a <= 0 (char is negative?)
                        // Wait, previous logic was: if (-val_a <= 0) which is val_a >= 0
                        if ($signed(mem_din) >= 0) begin
                            pc <= addr_c;
                            state <= (addr_c == 32'hFFFFFFFF) ? HALT : FETCH_A;
                        end else begin
                            pc <= pc + 3;
                            state <= FETCH_A;
                        end
                    end else begin
                        mem_addr <= addr_b;
                        state    <= EXECUTE;
                    end
                end

                EXECUTE: begin
                    sub_result = mem_din - val_a;
                    
                    mem_addr <= addr_b;
                    mem_dout <= sub_result;
                    mem_we   <= 1;

                    if ($signed(sub_result) <= 0) begin
                        pc <= addr_c;
                        if (addr_c == 32'hFFFFFFFF) state <= HALT;
                        else state <= FETCH_A;
                    end else begin
                        pc <= pc + 3;
                        state <= FETCH_A;
                    end
                end

                HALT: begin
                    halted <= 1;
                end
                
                default: begin
                    state <= HALT;
                end
            endcase
            
            // Global halt check
            if (pc == 32'hFFFFFFFF) state <= HALT;
        end
    end

endmodule
