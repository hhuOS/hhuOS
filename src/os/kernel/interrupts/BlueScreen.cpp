/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#include <kernel/Bios.h>
#include <kernel/memory/MemLayout.h>
#include <lib/graphic/Color.h>
#include <lib/libc/printf.h>
#include <kernel/KernelSymbols.h>
#include "BlueScreen.h"

const char *BlueScreen::errorMessage = "";

BlueScreen::BlueScreen(uint16_t columns, uint16_t rows) : columns(columns), rows(rows) {

}

void BlueScreen::print(InterruptFrame &frame) {

    printf("\n\n  [PANIC] %s\n\n", Cpu::getExceptionName(frame.interrupt));

    uint32_t *ebp = (uint32_t*) frame.ebp;

    uint32_t eip = frame.eip;

    uint32_t i = 0;

    while (eip) {

        printf("     #%02d 0x%08x --- %s\n", i, eip, KernelSymbols::get(eip));

        eip = ebp[1];

        ebp = (uint32_t*) ebp[0];

        if ((uint32_t ) ebp < KERNEL_START) {

            break;
        }

        i++;
    }

    printf("\n     %s\n\n", errorMessage);

    printf("     eax=0x%08x  ebx=0x%08x  ecx=0x%08x  edx=0x%08x\n", frame.eax, frame.ebx, frame.ecx, frame.edx);
    printf("     esp=0x%08x  ebp=0x%08x  esi=0x%08x  edi=0x%08x\n\n", frame.esp, frame.ebp, frame.esi, frame.edi);
    printf("     eflags=0x%08x", frame.eflags);
}

void BlueScreen::puts(const char *s, uint32_t n) {
    for(uint32_t i = 0; i < n; i++) {
        putc(s[i]);
    }
}

void BlueScreen::putc(const char c) {

    if(y >= rows) {
        return;
    }

    if(c == '\n') {
        x = 0;
        y++;
    } else {
        show(x, y, c);
        x++;
    }

    if(x >= columns) {
        x = 0;
        y++;
    }
}

void BlueScreen::flush() {
    puts(StringBuffer::buffer, StringBuffer::pos);
    pos = 0;
}

void BlueScreen::setErrorMessage(const char *message) {
    errorMessage = message;
}


