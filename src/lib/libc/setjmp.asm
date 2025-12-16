; Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
; Institute of Computer Science, Department Operating Systems
; Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
; Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
; This project has been supported by several students.
; A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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
;
; The stanard C library is based on a master's thesis, written by Tobias Fabian Oehme.
; The original source code can be found here: https://github.com/ToboterXP/hhuOS/tree/thesis

[GLOBAL _setjmp]
[GLOBAL _longjmp]

[SECTION .text]
[BITS 32]

_setjmp:
	pop edx	; Get return address into edx
	pop ecx	; Get address of jump_struct into ecx
	
	; Store registers into jump_struct
	mov [ecx], esp
	mov [ecx+4], ebp
	mov [ecx+8], edx 
	
	mov eax, 0
	push edx
	ret

_longjmp:
	pop ecx	; Get return address off the stack
	pop ecx	; Address of jump_struct in ecx
	pop eax	; Status to return into eax
	
	; If status is zero, return 1 instead
	cmp eax, 0
	jne status_not_zero
	mov eax, 1

status_not_zero:
	mov esp, [ecx]
	mov ebp, [ecx+4]
	mov edx, [ecx+8]
	
	push edx
	ret