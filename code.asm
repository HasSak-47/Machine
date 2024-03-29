section .text:
_start:
	IO msg 0 14
	IO local 1 10
	IO local 0 10
	END

section .data:
	msg: "hello world!" 10 0
	local: 0
