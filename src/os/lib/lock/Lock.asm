global compareAndSwap

section .text

compareAndSwap:

    push ebp
	mov ebp, esp

	mov eax, [ebp + 0x0C]
    mov ecx, [ebp + 0x08]
	mov edx, [ebp + 0x10]

	lock cmpxchg [ecx], edx

	jnz swapFailed

    mov esp, ebp
    pop ebp

    mov eax, 1

    ret

swapFailed:

    mov esp, ebp
    pop ebp

    mov eax, 0

    ret
