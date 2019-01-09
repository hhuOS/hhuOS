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
#include <kernel/bluescreen/BlueScreen.h>
#include <kernel/interrupts/InterruptManager.h>
#include <kernel/memory/SystemManagement.h>
#include "Cpu.h"

extern "C" {
    void onException(uint32_t exception);
}

// lists of Exceptions that can occur
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
        "OutOfPageTableMemory Exception", "IllegalPageAccess Exception", "UnknownType Exception",
        "PagingError Exception", "UnsupportedOperation Exception"
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

bool Cpu::isInterrupted() {

    uint32_t eflags;

    asm volatile (  "pushf\n"
                    "pop %0" : "=a"(eflags) );

    return (eflags & 0x200) == 0;
}

void Cpu::enableInterrupts() {

    cliCount--;

    if (cliCount < 0) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    if (cliCount == 0) {

        asm volatile ( "sti" );

        if(SystemManagement::isInitialized()) {

            InterruptManager::getInstance().handleDisabledInterrupts();
        }
    }
}

void Cpu::disableInterrupts() {

    asm volatile ( "cli" );

    cliCount++;
}

void Cpu::idle () {
    asm volatile ( "sti\n"
                   "hlt"
    );
}

void Cpu::halt () {
    asm volatile ( "cli\n"
                   "hlt"
    );
    __builtin_unreachable();
}

unsigned long long int Cpu::rdtsc() {
    unsigned long long int  ret;
    asm volatile ( "rdtsc" : "=A"(ret) );
    return ret;
}

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

void Cpu::throwException(Exception exception, const char *message) {

    disableInterrupts();

    BlueScreen::setErrorMessage(message);

    onException((uint32_t) exception);
}

void Cpu::softInterrupt(uint32_t function) {
    asm volatile ( "movl %0, %%eax;"
                   "int $0x80;"
                   : : "r"(function) );
}
