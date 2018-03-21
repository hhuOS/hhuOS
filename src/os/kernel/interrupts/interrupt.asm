; all assembler code regarding interrupts is placed here
; i.e. wrapper bodies, bios-call code and IDT

; Original IDT and wrapper by Original by Olaf Spinczyk, TU Dortmund.
; Refactored and extended by
; Michael Schoettner, Fabian Ruhland, Filip Krakowski, Burak Akguel, Christian Gesse, HHU 2017

%include "kernel/constants.asm"

[GLOBAL bios_call]
[GLOBAL setup_idt]
[GLOBAL interrupt_return]
[GLOBAL enable_interrupts]
[GLOBAL disable_interrupts]
[GLOBAL setSchedInit]
[GLOBAL onException]


[EXTERN preempt]
[EXTERN int_disp]
[EXTERN switch_context]
[EXTERN gdt_48]
[EXTERN _gdt_bios_48]
[EXTERN BIOS_Page_Directory]
[EXTERN stack]

; some low addresses of labels - use if paging disabled
_bios_call2             equ (bios_call2 - KERNEL_START)
_bios_call3             equ (bios_call3 - KERNEL_START)
_BIOS_Page_Directory    equ (BIOS_Page_Directory - KERNEL_START)

[SECTION .text]

enable_interrupts:
    sti
    ret

disable_interrupts:
    cli
    ret

; handle exceptions
onException:
    push ebp
    mov ebp, esp

    pushfd
    push cs
    push dword [ebp + 0x04]
    push 0x0
    push dword [ebp + 0x08]

    jmp wrapper_body


; bios_call
;
; procedure is splitted into different parts, because paging must be
; disabled
;
bios_call:
; load bios-call IDT
    lidt    [idt16_descr]
; safe registers
    pushfd
    pushad

; check if scheduler is started (we have to switch the stack then,
; because bios calls expext the stack to be placed at 4mb)
    mov ebx, [schedInit]
    cmp ebx, 0
    je skipStackSwitch

; switch stack to startup stack used in startup.asm, safe current stack pointer
; this is necessary because this stack is used for the bios call without paging
; and therefore we must know its physical address (which is CurrentStackAddress - 0xC0000000)
    mov ebx, stack
    add ebx, STACK_SIZE
    sub ebx, 4
    mov [ebx], esp
    mov esp, ebx

skipStackSwitch:

; save address of current Page Directory
    mov ecx, cr3
    push ecx

; enable 4mb-Paging
    mov ecx, cr4
    or ecx, 0x00000010
    mov cr4, ecx

; load special 4mb-Page Directory for BIOS-calls
    mov ecx, _BIOS_Page_Directory
    mov cr3, ecx

; jump to low address because paging will be disabled
; kernel should be mapped at 0 and 3GB
; necessary step: otherwise EIP points to wrong address
    mov ecx, _bios_call2
    jmp ecx

bios_call2:
; disable paging because we have to switch into rela mode
    mov ecx, cr0
    and ecx, 0x7FFFFFFF
    mov cr0, ecx
; flush TLB
    mov ecx, cr3
    mov cr3, ecx
; load gdt for bios calls (-> low addresses)
    lgdt [_gdt_bios_48]

; for calculation
    mov edx, KERNEL_START
; Shift values of some registers to low addresses because paging is disabled
    mov ecx, esp
    sub ecx, edx
    mov esp, ecx

    mov ecx, ebp
    sub ecx, edx
    mov ebp, ecx

bios_call3:
; jump into BIOS-Segment
    call  0x18:0
; code does not reach this point now - something happens in realmode,
; so that code does not return
; enable Paging
    mov ecx, cr0
    or ecx, 0x80000000
    mov cr0, ecx
; load global descriptor table
    lgdt [gdt_48]
; far jump to high address (paging on)
    lea ecx, [bios_call4]
    jmp ecx

bios_call4:
; shift values of some registers to high addresses for paging
    mov edx, KERNEL_START
    mov ecx, esp
    add ecx, edx
    mov esp, ecx

    mov ecx, ebp
    add ecx, edx
    mov ebp, ecx
; load page table of process and enable 4kb paging
    pop ecx
    mov cr3, ecx
; check if scheduler is active -> old stack has to be restored then
    mov ebx, [schedInit]
    cmp ebx, 0
    je skipStackSwitch2
; restore old stack
    pop esp

skipStackSwitch2:
; switch off 4mb Paging
    mov ecx, cr4
    and ecx, 0xFFFFFFEF
    mov cr4, ecx
; restore old register values
    popad
    popfd
; load old IDT
    lidt	[idt_descr]
    ret

; is called when scheduler starts
setSchedInit:
    push ecx
    mov ecx, 1
    mov [schedInit], ecx
    pop ecx
    ret

;
; setup_idt
;
; Relocation of IDT-entries; set IDTR

setup_idt:
	mov	eax,wrapper_0	; ax: lower 16 Bit
	mov	ebx,eax
	shr	ebx,16      ; bx: upper 16 Bit
	mov	ecx,255     ; Counter
.loop:
    add	[idt+8*ecx+0],ax
	adc	[idt+8*ecx+6],bx
	dec	ecx
	jge	.loop

	lidt	[idt_descr]
	ret


; wrapper for interrupt-handling

%macro wrapper 1
wrapper_%1:

    push    0x00
    push    %1
    jmp	wrapper_body
%endmacro


; create first 14 wrappers
%assign i 0
%rep 14
wrapper i
%assign i i+1
%endrep

; Page-Fault wrapper is different, because error code ist pushed
wrapper_14:

    push    0x0E
    jmp	wrapper_body

; create all remaining wrappers
%assign i 15
%rep 241
wrapper i
%assign i i+1
%endrep

; unique body for all wrappers
wrapper_body:

    ; Save state
    pushad

    push    ds
    push    es
    push    fs
    push    gs

    cld

    mov     ax, 0x10
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax

    ; Call interrupt handler
    push    esp
    call	int_disp
    add     esp, 0x04

interrupt_return:

    ; Load new state
    pop     gs
    pop     fs
    pop     es
    pop     ds

    popad

    ; Remove error code and interrupt number
    add     esp, 0x08
    iret




[SECTION .data]

idt_descr:
	dw	256*8-1     ; idt contains 256 entries
	dd	idt

;
; IDT for Realmode ;
; (Michael Schoettner)
;
idt16_descr:
    dw	1024    ; idt contains max. 1024 entries
    dd	0       ; address 0



;  create IDT with 256 entries

idt:

%macro idt_entry 1
	dw	(wrapper_%1 - wrapper_0) & 0xffff
	dw	0x0008
	dw	0x8e00
	dw	((wrapper_%1 - wrapper_0) & 0xffff0000) >> 16
%endmacro

; use macro

%assign i 0
%rep 256
idt_entry i
%assign i i+1
%endrep

; status of scheduler
schedInit:
    dw 0
