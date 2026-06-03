.PHONY: all build test clean

all: build test

build:
	cabal build

test:
	cabal test lexer-test --test-show-details=direct
	cabal test assembler-test --test-show-details=direct
	cabal test emu-test --test-show-details=direct
	cabal test e2e-test --test-show-details=direct

clean:
	cabal clean
	rm -f tests/assembler/*.json
	rm -f tests/emu/*.json
	rm -f tests/end_to_end/*.json
