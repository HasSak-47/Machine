section .text:
_start:
	MOV r0 0 ; mov 0 to r0
	MOV r1 1 ; mov 1 to r1
	_while:
		ADD         ; add r0 and r1 and store in r2
		MOV r0 r2   ; mov result to r0
		MOV r3 r1   ; mov to r3 r1 to save it
		MOV r1 10
		CMP         ; cmp r1 and 10
		JMP_EQ _end ; if r1 == 10 jump to _end
		MOV r1 r3   ; restore r1
		JMP _while

	_end:
	END

section .data:
	test_string: "Hello, World!\n"
	test_values: 0 1 2 3 4 5 6 7 8 9 A B C D E F
