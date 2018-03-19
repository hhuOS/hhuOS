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
;* Autor:           Olaf Spinczyk, TU Dortmund                               *
;*                  Michael Schoettner, HHU, 25.8.2016                       *
;*****************************************************************************

; Alle Register des Intel 80386 Prozessors

; Die folgenden Angaben belegen ebx_offset mit dem Wert 0, esi_offset mit 4
; edi_offset mit 8 usw. Dies entspricht den Abstaenden der Elemente ebx, esi,
; edi usw. der Struktur ThreadState zum Anfang der Struktur (in Bytes).

[ABSOLUTE 0]
gs_offset:      resw 1
pad6:           resw 1
fs_offset:      resw 1
pad5:           resw 1
es_offset:      resw 1
pad4:           resw 1
ds_offset:      resw 1
pad3:           resw 1
edi_offset:	    resd 1
esi_offset:	    resd 1
ebp_offset:	    resd 1
esp_offset:	    resd 1
ebx_offset:	    resd 1
edx_offset:	    resd 1
ecx_offset:	    resd 1
eax_offset:	    resd 1
int_offset:	    resd 1
err_offset:	    resd 1
eip_offset:	    resd 1
cs_offset:	    resw 1
pad2:           resw 1
efl_offset:	    resd 1
uesp_offset:    resd 1
ss_offset:      resw 1
pad1:           resw 1