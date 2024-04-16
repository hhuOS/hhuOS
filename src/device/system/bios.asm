[GLOBAL bios_call]
[GLOBAL bios_call_16_start]
[GLOBAL bios_call_16_end]
[GLOBAL bios_call_16_interrupt]

BIOS_CALL_CODE equ 0x00000500
BIOS_CALL_STACK equ 0x00000800

[SECTION .text]
[BITS 32]

; Execute a protected mode BIOS call (Using 0x500 as entry point allows executing real mode BIOS calls)
bios_call:
    ; Store registers
    pushad
    pushfd
    push gs
    push fs
    push es
    push ds

    ; Switch stack to prepared stack for APM call
    mov [esp_backup],esp ; Store ESP
    mov edx,[(esp + 52) + 8] ; Store second parameter (entry point) in EDX
    mov esp,[(esp + 52) + 4] ; First parameter (Pointer to prepared stack for BIOS call)
    push edx ; Store entry point on new stack

    ; Pop parameters from the struct (now on stack) into the CPU registers
    add esp,4 ; Do not pop entry point
    pop ds
    pop es
    pop fs
    pop gs
    pop eax ; Only to skip flags
    popad

    ; Disable paging and backup CR3 on the stack
    push eax
    mov eax,cr0
    and eax,0x7fffffff
    mov cr0,eax
    mov eax,cr3
    push eax
    mov eax,0x00000000
    mov cr3,eax
    mov eax,[esp + 4]

    ; Execute call
    push 0x08 ; Kernel code segment
    push bios_call_return ; Next instruction after BIOS call
    push 0x18 ; Code segment for BIOS call
    push dword [esp - 36] ; Entry point for BIOS function
    retf ; Call BIOS function

; We return here from BIOS code
bios_call_return:
    ; Restore CR3
    push eax
    mov eax,[esp + 4]
    mov cr3,eax
    pop eax

    ; Push registers into the parameter struct, which then holds return values from the BIOS call
    mov esp,BIOS_CALL_STACK
    pushad
    pushfd ; Do not skip flags this time
    push gs
    push fs
    push es
    push ds

    ; Enable paging
    mov eax,cr0
    or eax,0x80000000
    mov cr0,eax

    ; Restore esp
    mov esp,[esp_backup]

    ; Restore registers
    pop ds
    pop es
    pop fs
    pop gs
    popfd
    popad

    ret

[BITS 16]
; Leave protected mode and call BIOS function via interrupt in real mode
bios_call_16_start:
    ; Load data segment
    push dx
    mov dx,0x20
    mov ds,dx
    mov es,dx
    mov fs,dx
    mov gs,dx
    mov ss,dx
    pop dx

    ; Turn of protected mode via cr0 (also disable write protection)
    push eax
    mov eax,cr0
    and eax,0x7ffefffe
    mov cr0,eax
    pop eax

    ; Flush pipeline and switch decoding unit to real mode by performing a far jump to the next instruction
    jmp 0x0000:(BIOS_CALL_CODE + bios_call_16_real_mode_enter - bios_call_16_start)

bios_call_16_real_mode_enter:
    ; Setup segment registers
    push dx
    mov dx,0x0000
    mov ds,dx
    mov es,dx
    mov fs,dx
    mov gs,dx
    mov ss,dx
    pop dx

; Perform BIOS call (value is manually placed into memory by Bios::interrupt())
bios_call_16_interrupt:
    int 0x00

    ; Disable interrupts (might have been enabled by the BIOS)
    cli

    ; Enable protected mode (without paging) and write protection
    push eax
    mov eax,cr0
    or eax,0x00010001
    mov cr0,eax
    pop eax

    ; Flush pipeline and switch decoding unit to protected mode by performing a far jump to the next instruction
    jmp 0x0018:(BIOS_CALL_CODE + (bios_call_16_real_mode_leave - bios_call_16_start))

bios_call_16_real_mode_leave:
    ; Restore stack segment
    push dx
    mov dx,0x0010
    mov ss,dx
    pop dx

    ; Far return to bios_call_return:
[BITS 32]
    retfw
bios_call_16_end:

[SECTION .data]
esp_backup dw 0x00000000