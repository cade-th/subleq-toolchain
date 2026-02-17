4. Negative number support in lexer
6. Undefined label handling (currently defaults to 0)
8. Fix rigid 3-operand parsing structure (data definitions)

git ls-files | grep -E '\.(c|h)$' | xargs wc -l

./assembler/build/subleq assembler/asm/fib_macro.cade
./emu/build/6502_emu output.bin