#ifndef SUBLEQ_CPU_HPP
#define SUBLEQ_CPU_HPP

#include <vector>

class CPU {
public:
    int pc = 0;
    std::vector<int> memory;
    int total_cycles = 0;

    CPU() = default;

    int subleq();
    void step();
    void run(int cycles);
    void load_program(const std::vector<int>& input_program);
    void reset();
};

#endif
