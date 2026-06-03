.PHONY: test

test:
	export PYTHONPATH=".:$$PYTHONPATH" && \
	python3 tests/assembler/lexer_test.py && \
	python3 tests/assembler/assembler_test.py && \
	python3 tests/emu/emu_test.py && \
	python3 tests/end_to_end/end_to_end_test.py
