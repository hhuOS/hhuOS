/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef __ThreadState_include__
#define __ThreadState_include__

#include <cstdint>

namespace Kernel {

struct Context {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebx;
    uint32_t ebp;
    uint32_t eip;
} __attribute__((packed));

struct InterruptFrame {
    uint16_t gs;
    uint16_t pad6;
    uint16_t fs;
    uint16_t pad5;
    uint16_t es;
    uint16_t pad4;
    uint16_t ds;
    uint16_t pad3;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t interrupt;
    uint32_t error;
    uint32_t eip;
    uint16_t cs;
    uint16_t pad2;
    uint32_t eflags;
    uint32_t uesp;
    uint16_t ss;
    uint16_t pad1;
} __attribute__((packed));

}

#endif