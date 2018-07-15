; Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
; Heinrich-Heine University
;
; This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
; License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
; later version.
;
; This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
; warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
; details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>

; Assembler code for the Spinlock
; see Spinlock.h und Spinlock.cpp
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
	xchg eax, [ecx]	        ; swap values of lock-variable and eax in an atomic step -> lock-variable is 1 after this step in every case
	test eax, eax			; bitwise AND of eax against itself -> set zero flag if result is 0 (this is the case if 0 was in the lock-var before)
	jnz spin			    ; spin-loop if zero flag is not set -> in this case the result of the test-operation was not zero

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
	xchg eax, [ecx]	; swap values of lock-variable and eax in an atomic step -> lock-variable is 1 after this step in every case
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
	xchg eax, [ecx]	; set lock-variable to 0 in an atomic step

    pop ecx
    pop eax
	mov esp, ebp
	pop ebp					; restore ebp
	ret						; return
