;
; arch/ia32/user.asm
; Copyright (c) 2005 Luke McCarthy
;
; Test user process.
;

use32

global _user, _euser

section .rodata

_user:
	mov eax, [0x500000]
.L1:
	int 0x30
%if 1
	mov ecx, 400000
.L2:
	dec ecx
	jnz .L2
%endif
	jmp short .L1
_euser:
