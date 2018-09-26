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

#include <lib/String.h>
#include <kernel/interrupts/BlueScreen.h>
#include "Cpu.h"

extern "C" {
    void onException(uint32_t exception);
}

// lists of Exceptions that cann occur
const char* Cpu::hardwareExceptions[] = {
        "Divide-by-zero Error", "Debug", "Non-maskable Interrupt", "Breakpoint",
        "Overflow", "Bound Range Exceeded", "Invalid Opcode", "Device not available",
        "Double Fault", "Coprocessor Segment Overrun", "Invalid TSS", "Segment Not Present",
        "Stack-Segment Fault", "General Protection Fault", "Page Fault", "Reserved",
        "x87 Floating-Point Exception", "Alignment Check", "Machine Check", "SIMD Floating-Point Exception",
        "Virtualization Exception", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
        "Reserved", "Reserved", "Reserved", "Security Exception", "Reserved"
};

const char* Cpu::softwareExceptions[] {
        "NullPointer Exception", "IndexOutOfBounds Exception", "InvalidArgument Exception", "KeyNotFound Exception",
        "IllegalState Exception", "OutOfMemoryException", "OutOfPhysicalMemory Exception",
        "OutOfPageTableMemory Exception", "Illegal Page Access", "UnknownDriver Exception", "PagingError Exception"
};

int32_t Cpu::cliCount = 1; // GRUB disables all interrupts on startup

// enabling and disabling interrupts is done in assembler code
extern "C" {
    void enable_interrupts();
    void disable_interrupts();
};

void enable_interrupts() {

    Cpu::enableInterrupts();
}

void disable_interrupts() {

    Cpu::disableInterrupts();
}

/**
 * Checks if interrupt flag is set in EFLAGS.
 */
bool Cpu::isInterrupted() {

    uint32_t eflags;

    asm volatile (  "pushf\n"
                    "pop %0" : "=a"(eflags) );

    return (eflags & 0x200) == 0;
}

/**
 * Enables hardware interrupts on CPU.
 */
void Cpu::enableInterrupts() {

    cliCount--;

    if (cliCount < 0) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    if (cliCount == 0) {

        asm volatile ( "sti" );
    }
}

/**
 * Disables hardware interrupts on CPU.
 */
void Cpu::disableInterrupts() {

    asm volatile ( "cli" );

    cliCount++;
}

/**
 * Stop CPU unitl next interrupt.
 */
void Cpu::idle () {
    asm volatile ( "sti\n"
                   "hlt"
    );
}

/**
 * Stop the processor via hlt instruction.
 */
void Cpu::halt () {
    asm volatile ( "cli\n"
                   "hlt"
    );
    __builtin_unreachable();
}

/**
 * Reads the time stamp counter.
 */
unsigned long long int Cpu::rdtsc() {
    unsigned long long int  ret;
    asm volatile ( "rdtsc" : "=A"(ret) );
    return ret;
}

/**
 * Returns the name of the Exception from the ennumeration.
 */
const char *Cpu::getExceptionName(Cpu::Error exception) {

    uint32_t slot = (uint32_t) exception;

    if (slot >= SOFTWARE_EXCEPTIONS_START) {
        return softwareExceptions[slot - SOFTWARE_EXCEPTIONS_START];
    }

    return hardwareExceptions[slot];
}


const char *Cpu::getExceptionName(uint32_t exception) {

    if (exception >= SOFTWARE_EXCEPTIONS_START) {
        return softwareExceptions[exception - SOFTWARE_EXCEPTIONS_START];
    }

    return hardwareExceptions[exception];
}

/**
 * Throws an exception.
 */
void Cpu::throwException(Exception exception, const char *message) {

    disableInterrupts();

    BlueScreen::setErrorMessage(message);

    onException((uint32_t) exception);
}
