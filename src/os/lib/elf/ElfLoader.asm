section .text

global resolveSymbolHelper

extern resolveSymbol

resolveSymbolHelper:
    call    resolveSymbol
    pop     ecx
    pop     ecx
    jmp     eax

section .data
