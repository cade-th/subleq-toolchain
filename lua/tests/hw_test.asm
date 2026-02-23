#define print(char, next) char -1 next endm
START:
	print(H, NEXT)
NEXT:
	print(I, HALT)
HALT:
	0 0 -1
H: 72
I: 73
