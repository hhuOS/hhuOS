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

#include "kernel/interrupts/Pic.h"
#include "kernel/IOport.h"

static IOport PIC1_DATA(0x21);

static IOport PIC2_DATA(0xA1);

static IOport PIC1_COMMAND(0x20);

static IOport PIC2_COMMAND(0xA0);

Pic& Pic::getInstance()  {

    static Pic instance;

    return instance;
}

void Pic::allow(Pic::Interrupt interrupt) {

    IOport &port = getDataPort(interrupt);

    uint8_t mask = getMask(interrupt);

    port.outb(port.inb() & ~mask);
}

void Pic::forbid(Pic::Interrupt interrupt) {

    IOport &port = getDataPort(interrupt);

    uint8_t mask = getMask(interrupt);

    port.outb(port.inb() | mask);
}

bool Pic::status(Pic::Interrupt interrupt) {

    IOport &port = getDataPort(interrupt);

    uint8_t mask = getMask(interrupt);

    return port.inb() & mask;
}

void Pic::sendEOI(Pic::Interrupt interrupt) {
    if (interrupt >= Interrupt::RTC) {
        PIC2_COMMAND.outb(EOI);
    }

    PIC1_COMMAND.outb(EOI);
}

IOport& Pic::getDataPort(Pic::Interrupt interrupt) {

    if (interrupt >= Interrupt::RTC) {

        return PIC2_DATA;
    }

    return PIC1_DATA;
}

IOport& Pic::getCommandPort(Pic::Interrupt interrupt) {

    if (interrupt >= Interrupt::RTC) {

        return PIC2_COMMAND;
    }

    return PIC1_COMMAND;
}

uint8_t Pic::getMask(Pic::Interrupt interrupt) {

    if (interrupt >= Interrupt::RTC) {

        return (uint8_t ) (1 << ((uint8_t ) interrupt - 8));
    }

    return (uint8_t ) (1 << (uint8_t ) interrupt);
}

bool Pic::isSpurious() {
    PIC1_COMMAND.outb(READ_ISR);

    return (PIC1_COMMAND.inb() & SPURIOUS_INTERRUPT) == 0;
}
