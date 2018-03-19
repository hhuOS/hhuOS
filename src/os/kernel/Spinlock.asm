; Assembler code for the Spinlock
; see Spinlock.h und Spinlock.cc
; Burak Akguel and Christian Gesse, HHU, 2017


; export functions to C/C++ code
[GLOBAL acquireLock]
[GLOBAL releaseLock]
[GLOBAL tryAcquireLock]

[SECTION .text]

; entry point to lock
acquireLock:
	push ebp				; save ebp
	mov ebp, esp			; new stackframe
	push ecx				; save ecx
; entry point for spin-loop
spin:
	pause					; for CPU efficiency
	mov eax, 1				; load 1 for xchg
	mov ecx, [ebp+8]		; load address of lock-variable
	lock xchg eax, [ecx]	; swap values of lock-variable and eax in an atomic step -> lock-variable is 1 after this step in every case
	test eax, eax			; bitwise AND of eax against itself -> set zero flag if result is 0 (this is the case if 0 was in the lock-var before)
	jnz spin				; spin-loop if zero flag is not set -> in this case the reuslt of the test-operation was not zero

    pop ecx					; restore ecx
	mov esp, ebp
    pop ebp					; restore ebp
	ret						; return

; entry point for one try to acquire lock
tryAcquireLock:
    push ebp				; save ebp
	mov ebp, esp			; new stackframe
	push ecx

	pause					; for CPU efficiency
	mov eax, 1				; load 1 for xchg
	mov ecx, [ebp+8]		; load address of lock-variable
	lock xchg eax, [ecx]	; swap values of lock-variable and eax in an atomic step -> lock-variable is 1 after this step in every case
	test eax, eax			; bitwise AND of eax against itself -> set zero flag if result is 0 (this is the case if 0 was in the lock-var before)
	pushf		    		; push EFLAGS
	pop eax					; load value of EFLAGS into eax

	and eax, 0x40			; zero out everything except the zero flag

    pop ecx
    mov esp, ebp
    pop ebp					; restore ebp
	ret						; return


; entry point for unlock
releaseLock:
	push ebp				; save ebp
	mov ebp, esp			; new stackframe
	push eax				; save eax
	push ecx				; save ecx

	mov ecx, [ebp+8]		; load address of lock-variable
	mov eax, 0				; load 0 to eax
	lock xchg eax, [ecx]	; set lock-variable to 0 in an atomic step

    pop ecx
    pop eax
	mov esp, ebp
	pop ebp					; restore ebp
	ret						; return
