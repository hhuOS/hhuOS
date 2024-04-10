; Export functions
global _start
global _init
global _fini
global __cxa_pure_virtual

; Import functions
extern main
extern initMemoryManager
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
; Expects stack to be prepared with envp, argv, argc and heapStartAddress
_start:
    ; Initialize memory manager
    call initMemoryManager
    add esp, 4

    ; Initialize bss
    call clear_bss

    ; Initialize static variables
    call _init

    ; Call main method
    call main
    add esp,12
    push eax      ; Get return value from eax

    ; Cleanup static variables and exit process
    call _fini
    call _exit

; Zero out bss
clear_bss:
    mov    edi,___BSS_START__
clear_bss_loop:
    cmp    edi,___BSS_END__
    jge clear_bss_done
    mov    byte [edi],0
    inc    edi
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
    add    edi,4
    jmp _init_loop
_init_done:
    ret

; Call destructors of global objects
_fini:
    mov     edi,___FINI_ARRAY_START__
_fini_loop:
    cmp     edi,___FINI_ARRAY_END__
    jge _fini_done
    call [edi]
    add     edi,4
    jmp _fini_loop
_fini_done:
    ret

; This function is used when global constructors are called
; The label must be defined but can be void
__cxa_pure_virtual:
    ret