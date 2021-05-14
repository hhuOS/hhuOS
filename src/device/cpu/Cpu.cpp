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

#include <asm_interface.h>
#include "Cpu.h"

namespace Device {

const char *Cpu::hardwareExceptions[] = {
        "Divide-by-zero Error", "Debug", "Non-maskable Interrupt", "Breakpoint",
        "Overflow", "Bound Range Exceeded", "Invalid Opcode", "Device not available",
        "Double Fault", "Coprocessor Segment Overrun", "Invalid TSS", "Segment Not Present",
        "Stack-Segment Fault", "General Protection Fault", "Page Fault", "Reserved",
        "x87 Floating-Point Exception", "Alignment Check", "Machine Check", "SIMD Floating-Point Exception",
        "Virtualization Exception", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
        "Reserved", "Reserved", "Reserved", "Security Exception", "Reserved"
};

const char *Cpu::softwareExceptions[]{
        "NullPointer Exception", "IndexOutOfBounds Exception", "InvalidArgument Exception", "KeyNotFound Exception",
        "IllegalState Exception", "OutOfMemoryException", "OutOfPhysicalMemory Exception",
        "OutOfPageTableMemory Exception", "IllegalPageAccess Exception", "UnknownType Exception",
        "PagingError Exception", "UnsupportedOperation Exception"
};

Util::Async::Atomic<int32_t> Cpu::cliCount(1); // GRUB disables all interrupts on boot

void Cpu::enableInterrupts() {
    if (cliCount.fetchAndDec() == 0) {
        asm volatile ( "sti" );
    } else if (cliCount.get() < 0) {
        throwException(Util::Exception::ILLEGAL_STATE, "Cpu: cliCount is less than 0!");
    }
}

void Cpu::disableInterrupts() {
    asm volatile ( "cli" );
    cliCount.fetchAndInc();
}

void Cpu::halt() {
    asm volatile ( "cli\n"
                   "hlt"
    );
    __builtin_unreachable();
}

void Cpu::throwException(Util::Exception::Error error, const char *message) {
    disableInterrupts();
    on_exception((uint32_t) error);
}

void Cpu::softInterrupt(uint32_t function) {
    asm volatile ( "movl %0, %%eax;"
                   "int $0x80;"
    : : "r"(function));
}

}