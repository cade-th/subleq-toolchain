CXX = g++
CXXFLAGS = -std=c++11 -I./include -I.

TESTS = lexer_test assembler_test emu_test e2e_test

.PHONY: all test clean

all: $(TESTS)

lexer_test: tests/assembler/lexer_test.cpp lexer.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

assembler_test: tests/assembler/assembler_test.cpp assembler.cpp lexer.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

emu_test: tests/emu/emu_test.cpp emu/cpu.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

e2e_test: tests/end_to_end/end_to_end_test.cpp assembler.cpp lexer.cpp emu/cpu.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

test: $(TESTS)
	./lexer_test
	./assembler_test
	./emu_test
	./e2e_test

clean:
	rm -f $(TESTS)
