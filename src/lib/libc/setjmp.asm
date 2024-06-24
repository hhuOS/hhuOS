[GLOBAL _setjmp]
[GLOBAL _longjmp]

[SECTION .text]
[BITS 32]

_setjmp:
	pop edx	;Get return address into edx
	pop ecx	;Get address of jump_struct into ecx
	
	;store registers into jump_struct
	mov [ecx], esp
	mov [ecx+4], ebp
	mov [ecx+8], edx 
	
	mov eax, 0
	push edx
	ret
	


_longjmp:
	pop ecx	;Get return address off the stack
	pop ecx	;Address of jump_struct in ecx
	pop eax	;Status to return into eax 
	
	;if status is zero, return 1 instead
	cmp eax, 0
	jne status_not_zero
	mov eax, 1
status_not_zero:

	mov esp, [ecx]
	mov ebp, [ecx+4]
	mov edx, [ecx+8]
	
	push edx
	ret