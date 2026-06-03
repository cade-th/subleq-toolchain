.PHONY: test

test:
	@echo "Running Lexer Tests..."
	@cd tests/assembler && go run lexer_run.go
	@echo "Running Assembler Tests..."
	@cd tests/assembler && go run assembler_run.go
	@echo "Running Emulator Tests..."
	@cd tests/emu && go run emu_run.go
	@echo "Running End-to-End Tests..."
	@cd tests/end_to_end && go run end_to_end_run.go
