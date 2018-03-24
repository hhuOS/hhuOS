ENTRY(startup)
OUTPUT_FORMAT(elf32-i386)

SECTIONS
 {
   . = 0xC0100000;     /* Startadresse des Systems */

   ___KERNEL_START__ = .;

   .text : AT (ADDR(.text) - 0xC0000000)
    {
        ___TEXT_START__ = .;
        *(.text)
        ___TEXT_END__   = .;
    }

    .init_array : AT (ADDR(.init_array) - 0xC0000000)
    {
       ___INIT_ARRAY_START__ = .;
       KEEP (*(SORT(.init_array.*)))
       KEEP (*(.init_array))
       KEEP (*(.ctors))
       KEEP (*(.ctor))
       ___INIT_ARRAY_END__ = .;
    }

    .fini_array : AT (ADDR(.fini_array) - 0xC0000000)
    {
       ___FINI_ARRAY_START__ = .;
       KEEP (*(SORT(.fini_array.*)))
       KEEP (*(.fini_array))
       KEEP (*(.dtors))
       KEEP (*(.dtor))
       ___FINI_ARRAY_END__ = .;
    }

   .data ALIGN(0x1000): AT (ADDR(.data) - 0xC0000000)
    {
        ___DATA_START__ = .;
        *(.data)
        ___DATA_END__   = .;
    }

   .bss : AT (ADDR(.bss) - 0xC0000000)
    {
        ___BSS_START__  = .;
        *(.bss)
        ___BSS_END__    = .;
    }

    /DISCARD/ :
    {
        *(.note)
        *(.comment)
    }

    ___KERNEL_END__ = .;
}
