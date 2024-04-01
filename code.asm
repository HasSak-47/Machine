section .text:
_start:
	MOV r0 0
	MOV r1 a
_while:
	CMP
	JEQ _end
	MOV r3 r1 ; r3 = r1
	MOV r1 1  ; r1 = 1
	ADD       ; r0 + r1 => r2
	MOV r0 r2 ; r2 => r0
	MOV r1 r3 ; r3 => r1
	JMP _while

_end:
	END

section .data:
	holder: fa
