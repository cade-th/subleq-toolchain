package emu

import (
	"fmt"
)

type CPU struct {
	PC          int
	Memory      []int
	TotalCycles int
}

func NewCPU() *CPU {
	return &CPU{
		PC:          0,
		Memory:      nil,
		TotalCycles: 0,
	}
}

func (cpu *CPU) Subleq() int {
	if cpu.PC < 0 || cpu.PC+2 >= len(cpu.Memory) {
		return 0
	}

	addrA := cpu.Memory[cpu.PC]
	addrB := cpu.Memory[cpu.PC+1]
	addrC := cpu.Memory[cpu.PC+2]

	if addrB == -1 {
		if addrA >= 0 && addrA < len(cpu.Memory) {
			valA := cpu.Memory[addrA]
			fmt.Printf("%c", valA)

			if -valA <= 0 {
				cpu.PC = addrC
			} else {
				cpu.PC += 3
			}
		} else {
			return 0
		}
	} else {
		if addrA >= 0 && addrA < len(cpu.Memory) && addrB >= 0 && addrB < len(cpu.Memory) {
			valA := cpu.Memory[addrA]
			valB := cpu.Memory[addrB]
			result := valB - valA
			cpu.Memory[addrB] = result

			if result <= 0 {
				cpu.PC = addrC
			} else {
				cpu.PC += 3
			}
		} else {
			return 0
		}
	}

	return 1
}

func (cpu *CPU) Step() {
	cpu.TotalCycles += cpu.Subleq()
}

func (cpu *CPU) Run(cycles int) {
	for cpu.TotalCycles < cycles && cpu.PC >= 0 {
		cpu.Step()
	}
}

func (cpu *CPU) LoadProgram(input []int) {
	cpu.Memory = make([]int, len(input))
	copy(cpu.Memory, input)
	cpu.PC = 0
	cpu.TotalCycles = 0
}

func (cpu *CPU) Reset() {
	cpu.PC = 0
	cpu.TotalCycles = 0
}
