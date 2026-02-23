.PHONY: sim
VERILATOR = verilator
V_FLAGS = --cc --exe --build -j --trace

sim:
	$(VERILATOR) $(V_FLAGS) subleq.sv sim/main.cpp -o ../subleq_sim

clean:
	rm -rf obj_dir subleq_sim
