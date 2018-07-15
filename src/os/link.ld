ENTRY(startup)
OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
    . = 0xC0100000;     /* Virtual start address */

    .isa_dma ALIGN (64k) : AT (ADDR(.isa_dma) - 0xC0000000)
    {
        ___ISA_DMA_START__ = .;
       . = . + 64k;
        ___ISA_DMA_END__   = .;
    }

    ___KERNEL_START__ = .;
    ___WRITE_PROTECTED_START__ = .;

    .text ALIGN (4K) : AT (ADDR(.text) - 0xC0000000)
    {
        ___TEXT_START__ = .;
        *(.text)
        *(.text.*)
        ___TEXT_END__   = .;
    }

    .rodata ALIGN (4K) : AT(ADDR(.rodata)-0xC0000000)
    {
        ___RODATA_START__ = .;
        *(.rodata)
        *(.rodata.*)
        ___RODATA_END__ = .;
    }
    
    .init_array ALIGN (4K) : AT (ADDR(.init_array) - 0xC0000000)
    {
       ___INIT_ARRAY_START__ = .;
       KEEP (*(SORT(.init_array.*)))
       KEEP (*(.init_array))
       KEEP (*(.ctors))
       KEEP (*(.ctor))
       ___INIT_ARRAY_END__ = .;
    }

    .fini_array ALIGN (4K) : AT (ADDR(.fini_array) - 0xC0000000)
    {
       ___FINI_ARRAY_START__ = .;
       KEEP (*(SORT(.fini_array.*)))
       KEEP (*(.fini_array))
       KEEP (*(.dtors))
       KEEP (*(.dtor))
       ___FINI_ARRAY_END__ = .;
    }
    
    ___WRITE_PROTECTED_END__ = .;

   .data ALIGN (4K) : AT (ADDR(.data) - 0xC0000000)
    {
        ___DATA_START__ = .;
        *(.data)
        *(.data.*)
        ___DATA_END__   = .;
    }

    .bss ALIGN (4K) : AT (ADDR(.bss) - 0xC0000000)
    {
        ___BSS_START__  = .;
        *(.bss)
        *(.bss.*)
        ___BSS_END__    = .;
    }

    ___KERNEL_END__ = .;

    /DISCARD/ :
    {
        *(.note.gnu.build-id)
        *(.comment)
        *(.eh_frame)
    }
}
