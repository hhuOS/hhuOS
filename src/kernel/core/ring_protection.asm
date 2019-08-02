global flush_tss

flush_tss:
    mov ax, 0x28
    ltr ax
    ret