/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "asm_interface.h"
#include "lib/util/system/System.h"
#include "Cpu.h"
#include "lib/util/async/Atomic.h"
#include "lib/util/data/ArrayList.h"
#include "lib/util/data/Collection.h"
#include "lib/util/data/Iterator.h"

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

int32_t Cpu::cliCount = 1; // Interrupts are disabled on startup

void Cpu::enableInterrupts() {
    auto cliCountWrapper = Util::Async::Atomic<int32_t>(cliCount);
    int count = cliCountWrapper.fetchAndDec();

    if (count == 1) {
        // nmiCount has been decreased to 0 -> Enable interrupts
        asm volatile ( "sti" );
    } else if (count < 1) {
        // nmiCount has been decreased to a negative value -> Illegal state
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "CPU: nmiCount is less than 0!");
    }
}

void Cpu::disableInterrupts() {
    auto cliCountWrapper = Util::Async::Atomic<int32_t>(cliCount);
    int count = cliCountWrapper.fetchAndInc();

    if (count == 0) {
        // nmiCount has been increased from 0 to 1 -> Disable interrupts
        asm volatile ( "cli" );
    } else if (count < 0) {
        // nmiCount is negative -> Illegal state
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "CPU: nmiCount is less than 0!");
    }
}

void Cpu::halt() {
    asm volatile ( "cli\n"
                   "hlt"
    );
    __builtin_unreachable();
}

void Cpu::throwException(Util::Exception::Error error, const char *message) {
    disableInterrupts();
    Util::System::errorMessage = message;
    on_exception((uint32_t) error);
}

const char* Cpu::getExceptionName(uint32_t exception) {
    if (exception >= Util::Exception::NULL_POINTER) {
        return Util::Exception::getExceptionName(static_cast<Util::Exception::Error>(exception));
    }

    return hardwareExceptions[exception];
}

Util::Data::Array<Cpu::Configuration0> Cpu::readCr0() {
    auto cr0 = Util::Data::ArrayList<Configuration0>();
    uint32_t cr0Bits = 0;
    asm volatile (
            "mov %%cr0, %%eax;"
            "mov %%eax, (%0);"
            : :
            "r"(&cr0Bits)
            :
            "eax"
            );

    uint32_t i;
    for (i = 1; i <= Configuration0::CACHE_DISABLE; i *= 2) {
        if ((cr0Bits & i) != 0) {
            cr0.add(static_cast<Configuration0>(i));
        }
    }

    return cr0.toArray();
}

}