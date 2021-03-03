/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

ENTRY(boot)
OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
    . = 0xC0100000;     /* Virtual start address */

    ___KERNEL_DATA_START__ = .;

    .text ALIGN (4K) : AT (ADDR(.text) - 0xC0000000)
    {
        ___WRITE_PROTECTED_START__ = .;
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
    
    ___WRITE_PROTECTED_END__ = ALIGN(4K);

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

    ___KERNEL_DATA_END__ = .;

    /DISCARD/ :
    {
        *(.note.gnu.build-id)
        *(.comment)
        *(.eh_frame)
    }
}
