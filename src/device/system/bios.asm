[GLOBAL bios_call]
[GLOBAL bios_call_16_start]
[GLOBAL bios_call_16_end]
[GLOBAL bios_call_16_interrupt]

BIOS_CALL_CODE equ 0x00000500
BIOS_CALL_ESP_BACKUP equ 0x00000600
BIOS_CALL_STACK equ 0x00000700

[SECTION .text]
[BITS 32]

; Perform a bios call in real mode
; The procedure is split into different parts, because paging need to be disabled
bios_call:
    ; Store registers
    pushfd
    pushad

    ; Jump into bios segment
    call  0x18:BIOS_CALL_CODE

; We return here from 16-bit bios code
bios_call_16_return:
    ; Restore registers
    popad
    popfd

    ret

[BITS 16]
bios_call_16_start:
    ; Load data segment
    mov dx,0x20
    mov ds,dx
    mov es,dx
    mov fs,dx
    mov gs,dx
    mov ss,dx

    ; Save esp to 0x0ffc (last 4 bytes of memory reserved for the 16-bit code code)
    mov eax,BIOS_CALL_ESP_BACKUP
    mov [eax],esp

    ; Turn of protected mode via cr0 (also disable write protection)
    mov eax,cr0
    and eax,0x7ffefffe
    mov cr0,eax

    ; Flush pipeline and switch decoding unit to real mode by performing a far jump to the next instruction
    ; 0x0400:0x001b = (0x0400 << 4) + 0x001b = 0x4000 + 0x401b = 0x401b
    jmp 0x0000:(BIOS_CALL_CODE + bios_call_16_real_mode_enter - bios_call_16_start)

bios_call_16_real_mode_enter:
    ; Setup segment registers
    mov dx,0x0000
    mov ds,dx
    mov es,dx
    mov fs,dx
    mov gs,dx
    mov ss,dx

    ; Set esp to point directly to the context struct at address 0x6000
    mov esp,BIOS_CALL_STACK

    ; Enable interrupts
    sti

    ; Pop parameters from the struct (now on stack) into the CPU registers
    pop ds
    pop es
    pop fs
    pop ax ; Only to skip flags
    popad

    ; Perform BIOS call (value is manually placed into memory by Bios::interrupt())
bios_call_16_interrupt:
    int 0x00

    ; Push registers into the parameter struct, which then holds return values from the BIOS call
    pushad
    pushf ; Do not skip flags this time
    push fs
    push es
    push ds

    ; Disable interrupts
    cli

    ; Enable protected mode (without paging) and write protection
    mov eax,cr0
    or eax,0x00010001
    mov cr0,eax

    ; Flush pipeline and switch decoding unit to protected mode by performing a far jump to the next instruction
    ; 0x0018:0x0049 = GDT[0x0018] + 0x0049 = 0x4000 + 0x0049 = 0x4049
    jmp 0x0018:(BIOS_CALL_CODE + (bios_call_16_real_mode_leave - bios_call_16_start))

    ; Restore segment registers
bios_call_16_real_mode_leave:
    mov dx,0x0010
    mov ds,dx
    mov es,dx
    mov fs,dx
    mov gs,dx
    mov ss,dx

    ; Restore esp from 0x5000
    mov eax,BIOS_CALL_ESP_BACKUP
    mov esp,[eax]

    ; Far return to bios.asm
bits 32
    retfw

bios_call_16_end: