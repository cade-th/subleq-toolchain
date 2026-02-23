#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include "Vsubleq.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <program.hex> [trace.vcd]" << std::endl;
        return 1;
    }

    Vsubleq* top = new Vsubleq;
    VerilatedVcdC* tfp = nullptr;
    
    if (argc > 2) {
        tfp = new VerilatedVcdC;
        top->trace(tfp, 99);
        tfp->open(argv[2]);
    }

    std::vector<uint32_t> memory(65536, 0);

    // Load Hex file
    std::ifstream hex_file(argv[1]);
    if (!hex_file) {
        std::cerr << "Could not open " << argv[1] << std::endl;
        return 1;
    }

    std::string line;
    int addr = 0;
    while (std::getline(hex_file, line)) {
        memory[addr++] = std::stoul(line, nullptr, 16);
    }

    // Reset
    top->rst_n = 0;
    top->clk = 0;
    top->eval();
    top->clk = 1;
    top->eval();
    top->rst_n = 1;

    // Simulation loop
    int cycles = 0;
    while (!top->halted && cycles < 1000000) {
        top->clk = 0;
        top->eval();
        
        // Combinational memory read
        top->mem_din = memory[top->mem_addr];

        top->clk = 1;
        top->eval();

        // Sequential memory write
        if (top->mem_we) {
            memory[top->mem_addr] = top->mem_dout;
        }

        if (top->out_valid) {
            std::cout << (char)top->out_byte;
        }

        if (tfp) tfp->dump(cycles);
        cycles++;
    }

    if (tfp) {
        tfp->close();
        delete tfp;
    }

    if (cycles >= 1000000) {
        std::cout << "\nSimulation timeout!" << std::endl;
    } else {
        std::cout << "\nSimulation halted after " << cycles << " cycles." << std::endl;
    }

    delete top;
    return 0;
}
