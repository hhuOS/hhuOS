;*****************************************************************************
;*                                                                           *
;*                               T H R E A D                                 *
;*                                                                           *
;*---------------------------------------------------------------------------*
;* Beschreibung:    Assemblerdarstellung der struct ThreadState aus          *
;*                  ThreadState.h                                            *
;*                                                                           *
;*                  Die Reihenfolge der Registerbezeichnungen muss unbedingt *
;*                  mit der von struct ThreadState uebereinstimmen.          *
;*                                                                           *
;*                  Jetzt werden alle Register beruecksichtigt!              *
;*                                                                           *
;* Autor:           Olaf Spinczyk, TU Dortmund                               *
;*                  Michael Schoettner, HHU, 1.1.2017                        *
;*****************************************************************************

%include "kernel/threads/Thread.inc"

; EXPORTIERTE FUNKTIONEN

[GLOBAL Thread_start]
[GLOBAL Thread_switch]
[GLOBAL get_thread_vars]
[GLOBAL preempt]
[GLOBAL switch_context]

; IMPLEMENTIERUNG DER FUNKTIONEN

[SECTION .text]

; COROUTINE_START : Startet die erste Coroutine ueberhaupt.
;
; C Prototyp: void Coroutine_start (struct CoroutineState* regs);

Thread_start:

    ; Get thread context
    mov	esp, [esp + 0x04]

    pop edi
    pop esi
    pop ebx
    pop ebp

    ret



; THREAD_SWITCH : Threadumschaltung. Der aktuelle Registersatz wird
;                 gesichert und der Registersatz des neuen Threads
;                 wird in den Prozessor eingelesen.
;
; C Prototyp: void Thread_switch (struct ThreadState* regs_now,
;                                 struct ThreadState* reg_then);

Thread_switch:

    ; Save old state
    pushad
    pushfd

    mov	eax, [esp + 0x28]

    pop ecx
    mov [eax + efl_offset], ecx
    pop ecx
    mov [eax + edi_offset], ecx
    pop ecx
    mov [eax + esi_offset], ecx
    pop ecx
    mov [eax + ebp_offset], ecx
    pop ecx
    mov [eax + esp_offset], ecx
    pop ecx
    mov [eax + ebx_offset], ecx
    pop ecx
    mov [eax + edx_offset], ecx
    pop ecx
    mov [eax + ecx_offset], ecx
    pop ecx
    mov [eax + eax_offset], ecx
    pop ecx
    mov [eax + eip_offset], ecx

    mov [eax + gs_offset], gs
    mov [eax + fs_offset], fs
    mov [eax + es_offset], es
    mov [eax + ds_offset], ds
    mov [eax + cs_offset], cs

    ; Load new state

    mov	eax, [esp + 0x04]

    mov	ebx, [eax + ebx_offset]
    mov	esi, [eax + esi_offset]
    mov	edi, [eax + edi_offset]
    mov	esp, [eax + esp_offset]
    mov	ebp, [eax + ebp_offset]
    mov	ecx, [eax + ecx_offset]
    mov	edx, [eax + edx_offset]

    mov gs, [eax + gs_offset]
    mov fs, [eax + gs_offset]
    mov es, [eax + gs_offset]
    mov ds, [eax + gs_offset]

    push dword [eax + eip_offset]

    mov	eax, [eax + eax_offset]

    push dword [eax + efl_offset]
    popfd

    sti         ; Interrupts erlauben, evt. von Scheduler::block zuvor abgeschaltet
    ret         ; Threadwechsel !


; Auslesen der Thread-Vars (fuer erzwungenen Thread-Wechsel notwendig)
; C Prototyp: void get_thread_vars (unsigned int** regs_life,
;                                   unsigned int** reg_next);
get_thread_vars:
    push ebp
    mov ebp, esp

    push eax

    mov	eax, [ebp + 0x08]     ; regs_life
    mov dword [eax], regs_life
    mov	eax, [ebp + 0x0C]     ; regs_next
    mov dword [eax], regs_next

    pop eax

    mov esp, ebp
    pop ebp
    ret

switch_context:

    mov eax, [esp + 0x04]
    mov edx, [esp + 0x08]

    push ebp
    push ebx
    push esi
    push edi

    mov [eax], esp
    mov esp, [edx]

    pop edi
    pop esi
    pop ebx
    pop ebp

    sti

    ret

;
; Registersatz des aktuellen Threads sichern (in ThreadState-Struktur)
;
preempt:

    pop dword [preempt_return]

    ; Get old threads stack pointer
    pop esp

    ; Get the current threads state memory
    mov eax, [regs_life]

    ; Save old state
    pop ecx
    mov [eax + gs_offset], cx
    pop ecx
    mov [eax + fs_offset], cx
    pop ecx
    mov [eax + es_offset], cx
    pop ecx
    mov [eax + ds_offset], cx

    pop ecx
    mov [eax + edi_offset], ecx
    pop ecx
    mov [eax + esi_offset], ecx
    pop ecx
    mov [eax + ebp_offset], ecx

    ; ESP will be saved later
    add esp, 0x04

    pop ecx
    mov [eax + ebx_offset], ecx
    pop ecx
    mov [eax + edx_offset], ecx
    pop ecx
    mov [eax + ecx_offset], ecx
    pop ecx
    mov [eax + eax_offset], ecx
    pop ecx
    mov [eax + int_offset], ecx
    pop ecx
    mov [eax + err_offset], ecx
    pop ecx
    mov [eax + eip_offset], ecx
    pop ecx
    mov [eax + cs_offset], cx
    pop ecx
    mov [eax + efl_offset], ecx

    ; Save threads stack pointer
    mov [eax + esp_offset], esp

    ;-------------------------------------------------------------------------------------------------------------------

    ; Load next threads state
    mov eax, [regs_next]

    ; Load next threads stack pointer
    mov esp, [eax + esp_offset]
    mov ebp, [eax + ebp_offset]

    ; Read EFLAGS and set interrupt flag
    mov ecx, [eax + efl_offset]
    or  ecx, 0x200
    push ecx

    push dword [eax + cs_offset]
    push dword [eax + eip_offset]
    push dword [eax + err_offset]
    push dword [eax + int_offset]
    push dword [eax + eax_offset]
    push dword [eax + ecx_offset]
    push dword [eax + edx_offset]
    push dword [eax + ebx_offset]

    ; This will be ignored by POPAD, so it does not matter
    push dword [eax + esp_offset]

    push dword [eax + ebp_offset]
    push dword [eax + esi_offset]
    push dword [eax + edi_offset]

    push dword [eax + ds_offset]
    push dword [eax + es_offset]
    push dword [eax + fs_offset]
    push dword [eax + gs_offset]

    ; This will be taken off the stack after return
    push esp

    push dword [preempt_return]

    ret


[SECTION .data]
;
; Variablen fuer erzwungenen Thread-Wechsel
; (werden in der Unterbrechungsbehandlung des PIT gesetzt)
;
regs_life:
    db 0,0,0,0
regs_next:
    db 0,0,0,0

preempt_return:
    db 0,0,0,0