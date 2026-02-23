.PHONY: test

test:
	export LUA_PATH="./?.lua;;" && \
	lua tests/assembler/lexer_tests.lua && \
	lua tests/assembler/assembler_tests.lua && \
	lua tests/emu/emu_test.lua && \
	lua tests/end_to_end/end_to_end_tests.lua

sim:
	$(MAKE) -C hw sim

test-hw: sim
	export LUA_PATH="./?.lua;;" && \
	lua export_hex.lua < tests/hw_test.asm > tests/hw_test.hex && \
	./hw/subleq_sim tests/hw_test.hex
