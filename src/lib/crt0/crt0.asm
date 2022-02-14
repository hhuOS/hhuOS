; Export functions
global _start

; Import functions
extern main
extern _exit

; Import linker symbols
extern ___BSS_START__
extern ___BSS_END__
extern ___INIT_ARRAY_START__
extern ___INIT_ARRAY_END__
extern ___FINI_ARRAY_START__
extern ___FINI_ARRAY_END__

section .text

; Entry point
_start:
    ; Initialize bss and static variables
    call clear_bss
    call _init

    ; Setup environment for main
    push ecx        ; Push third parameter (envp) on the stack
    push ebx        ; Push second parameter (argv) on the stack
    push eax        ; Push first parameter (argc) on the stack

    call main

    ; Exit process
    push eax    ; Get return value from eax

    ; Cleanup static variables and exit process
    call _fini
    call _exit

; Zero out bss
clear_bss:
    mov	edi,___BSS_START__
clear_bss_loop:
    cmp	edi,___BSS_END__
    jge clear_bss_done
    mov	byte [edi],0
    inc	edi
    jmp clear_bss_loop
clear_bss_done:
    ret

; Call constructors of global objects
_init:
	mov edi,___INIT_ARRAY_START__
_init_loop:
	cmp edi,___INIT_ARRAY_END__
	jge _init_done
	call [edi]
	add	edi,4
	jmp _init_loop
_init_done:
	ret

; Call destructors of global objects
_fini:
    mov	 edi,___FINI_ARRAY_START__
_fini_loop:
    cmp	 edi,___FINI_ARRAY_END__
    jge _fini_done
    call [edi]
    add	 edi,4
    jmp _fini_loop
_fini_done:
    ret

; This function is used when global constructors are called
; The label must be defined but can be void
__cxa_pure_virtual:
    ret