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


#include "lib/util/base/System.h"
#include "Cpu.h"
#include "lib/util/async/Atomic.h"
#include "lib/util/collection/ArrayList.h"
#include "kernel/service/InterruptService.h"

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
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "CPU: nmiCount is less than 0!");
    }
}

void Cpu::disableInterrupts() {
    auto cliCountWrapper = Util::Async::Atomic<int32_t>(cliCount);
    int count = cliCountWrapper.fetchAndInc();

    if (count == 0) {
        // count has been increased from 0 to 1 -> Disable interrupts
        asm volatile ( "cli" );
    } else if (count < 0) {
        // count is negative -> Illegal state
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "CPU: nmiCount is less than 0!");
    }
}

void Cpu::halt() {
    asm volatile ( "cli\n"
                   "hlt"
    );
    __builtin_unreachable();
}

void Cpu::setSegmentRegister(SegmentRegister reg, const SegmentSelector &selector) {
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

void Cpu::loadTaskStateSegment(const Cpu::SegmentSelector &selector) {
    asm volatile(
            "ltr %0"
            : :
            "r"(static_cast<uint16_t>(selector))
            :
            );
}

Cpu::SegmentSelector::SegmentSelector(Cpu::PrivilegeLevel privilegeLevel, uint8_t index) :
    privilegeLevel(privilegeLevel), type(0), index(index) {}

Cpu::SegmentSelector::operator uint16_t() const {
    return privilegeLevel | type << 2 | index << 3;
}

}