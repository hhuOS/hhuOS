/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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


#include "Cpu.h"
#include "lib/util/async/Atomic.h"
#include "lib/util/base/Exception.h"

namespace Device {

int32_t Cpu::cliCount = 1; // Interrupts are disabled on startup

void Cpu::enableInterrupts() {
    auto cliCountWrapper = Util::Async::Atomic<int32_t>(cliCount);
    int count = cliCountWrapper.fetchAndDec();

    if (count == 1) {
        // count has been decreased to 0 -> Enable interrupts
        asm volatile ( "sti" );
    } else if (count < 1) {
        // count has been decreased to a negative value -> Illegal state
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "CPU: cliCount is less than 0!");
    }
}

void Cpu::disableInterrupts() {
    auto cliCountWrapper = Util::Async::Atomic<int32_t>(cliCount);
    int count = cliCountWrapper.fetchAndInc();

    if (count < 0) {
        // count is negative -> Illegal state
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "CPU: cliCount is less than 0!");
    }

    asm volatile ( "cli" );
}

void Cpu::halt() {
    asm volatile (
            "cli;"
            "hlt;"
    );

    __builtin_unreachable();
}

void Cpu::writeSegmentRegister(SegmentRegister reg, const SegmentSelector &selector) {
    switch (reg) {
        case CS:
            asm volatile(
                    "push %0;"
                    "push $cs_return;"
                    "retf;"
                    "cs_return:"
                    : :
                    "r"(static_cast<uint32_t>(static_cast<uint16_t>(selector)))
                    );
            break;
        case DS:
            asm volatile(
                    "mov %0, %%ds"
                    : :
                    "r"(static_cast<uint16_t>(selector))
                    );
            break;
        case ES:
            asm volatile(
                    "mov %0, %%es"
                    : :
                    "r"(static_cast<uint16_t>(selector))
                    );
            break;
        case FS:
            asm volatile(
                    "mov %0, %%fs"
                    : :
                    "r"(static_cast<uint16_t>(selector))
                    );
            break;
        case GS:
            asm volatile(
                    "mov %0, %%gs"
                    : :
                    "r"(static_cast<uint16_t>(selector))
                    );
            break;
        case SS:
            asm volatile(
                    "mov %0, %%ss"
                    : :
                    "r"(static_cast<uint16_t>(selector))
                    );
            break;
    }
}

Cpu::SegmentSelector Cpu::readSegmentRegister(Cpu::SegmentRegister reg) {
    uint16_t selector = 0;
    switch (reg) {
        case CS:
            asm volatile(
                    "mov %%cs, (%0)"
                    : :
                    "r"(&selector)
                    );
            break;
        case DS:
            asm volatile(
                    "mov %%ds, (%0)"
                    : :
                    "r"(&selector)
                    );
            break;
        case ES:
            asm volatile(
                    "mov %%es, (%0)"
                    : :
                    "r"(&selector)
                    );
            break;
        case FS:
            asm volatile(
                    "mov %%fs, (%0)"
                    : :
                    "r"(&selector)
                    );
            break;
        case GS:
            asm volatile(
                    "mov %%gs, (%0)"
                    : :
                    "r"(&selector)
                    );
            break;
        case SS:
            asm volatile(
                    "mov %%ss, (%0)"
                    : :
                    "r"(&selector)
                    );
            break;
    }

    return *reinterpret_cast<SegmentSelector*>(&selector);
}

uint32_t Cpu::readCr0() {
    uint32_t cr0 = 0;
    asm volatile (
            "mov %%cr0, %%eax;"
            "mov %%eax, (%0);"
            : :
            "r"(&cr0)
            :
            "eax"
            );

    return cr0;
}

void Cpu::writeCr0(uint32_t value) {
    asm volatile(
            "mov %0, %%cr0"
            : :
            "r"(value)
            :
            );
}

uint32_t Cpu::readCr2() {
    uint32_t cr2 = 0;
    asm volatile (
            "mov %%cr2, %%eax;"
            "mov %%eax, (%0);"
            : :
            "r"(&cr2)
            :
            "eax"
            );

    return cr2;
}

Kernel::Paging::Table *Cpu::readCr3() {
    uint32_t cr3 = 0;
    asm volatile (
            "mov %%cr3, %%eax;"
            "mov %%eax, (%0);"
            : :
            "r"(&cr3)
            :
            "eax"
            );

    return reinterpret_cast<Kernel::Paging::Table*>(cr3);
}

void Cpu::writeCr3(const Kernel::Paging::Table *pageDirectory) {
    asm volatile(
            "mov %0, %%cr3"
            : :
            "r"(pageDirectory)
            :
            );
}

void Cpu::loadTaskStateSegment(const Cpu::SegmentSelector &selector) {
    asm volatile(
            "ltr %0"
            : :
            "r"(static_cast<uint16_t>(selector)), "m"(selector)
            :
            );
}

Cpu::SegmentSelector::SegmentSelector(Cpu::PrivilegeLevel privilegeLevel, uint8_t index) :
    privilegeLevel(privilegeLevel), type(0), index(index) {}

Cpu::SegmentSelector::operator uint16_t() const {
    return privilegeLevel | type << 2 | index << 3;
}

}