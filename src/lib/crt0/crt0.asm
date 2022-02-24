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

%macro prepare 0
    push eax
    push ecx
    push edx
%endmacro

%macro cleanup 1
    add esp,%1
    pop edx
    pop ecx
    pop eax
%endmacro

%macro prologue 1
    push ebp
    mov ebp,esp
    sub esp,%1
    push ebx
    push edi
    push esi
%endmacro

%macro epilogue 0
    pop esi
    pop edi
    pop ebx
    mov esp,ebp
    pop ebp
    ret
%endmacro

; Entry point
_start:
    ; Initialize memory manager
    push ecx
    push ebx
    push eax

    push 0xbfffefff        ; Push second parameter (endAddress) on the stack
    push edx               ; Push first parameter (startAddress) on the stack
    call initMemoryManager
    add esp,8

    ; Initialize bss
    call clear_bss

    ; Initialize static variables
    call _init

    ; Call main method
    ;mov eax,[envp]  ; Push third parameter (envp) on the stack
    ;push eax
    ;mov eax,[argv]  ; Push second parameter (argv) on the stack
    ;push eax
    ;mov eax,[argc]  ; Push first parameter (argc) on the stack
    ;push eax
    call main
    add esp,12
    push eax      ; Get return value from eax

    ; Cleanup static variables and exit process
    call _fini
    call _exit

; Zero out bss
clear_bss:
    prologue 0
    mov	edi,___BSS_START__
clear_bss_loop:
    cmp	edi,___BSS_END__
    jge clear_bss_done
    mov	byte [edi],0
    inc	edi
    jmp clear_bss_loop
clear_bss_done:
    epilogue
    ret

; Call constructors of global objects
_init:
    prologue 0
	mov edi,___INIT_ARRAY_START__
_init_loop:
	cmp edi,___INIT_ARRAY_END__
	jge _init_done
	call [edi]
	add	edi,4
	jmp _init_loop
_init_done:
    epilogue
	ret

; Call destructors of global objects
_fini:
    prologue 0
    mov	 edi,___FINI_ARRAY_START__
_fini_loop:
    cmp	 edi,___FINI_ARRAY_END__
    jge _fini_done
    call [edi]
    add	 edi,4
    jmp _fini_loop
_fini_done:
    epilogue
    ret

; This function is used when global constructors are called
; The label must be defined but can be void
__cxa_pure_virtual:
    ret

section .data
    argc: dd 0
    argv: dd 0
    envp: dd 0
    main_ret:  dd 0