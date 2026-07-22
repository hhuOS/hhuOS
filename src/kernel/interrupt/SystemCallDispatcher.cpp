/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include "SystemCallDispatcher.h"

#include "device/cpu/ModelSpecificRegister.h"
#include "kernel/log/Log.h"

namespace Kernel {

void(*SystemCallDispatcher::systemCalls[256])();

void SystemCallDispatcher::enableFastSystemCalls() {
    Device::ModelSpecificRegister(IA32_SYSENTER_CS).writeQuadWord(0x8);
    Device::ModelSpecificRegister(IA32_SYSENTER_EIP).writeQuadWord(reinterpret_cast<uint64_t>(dispatchFast));
}

void SystemCallDispatcher::dispatch() {
    asm volatile (
        // Push parameters for the system call
        "push %%edi;"
        "push %%esi;"
        "push %%edx;"
        "push %%ecx;"
        "push %%ebx;"

        // Call the system call
        "call *%P0(,%%eax,4);"

        // Return to user mode
        "add $20, %%esp;"
        "iret;"
        :
        : "i"(systemCalls)
        );
}

void SystemCallDispatcher::dispatchFast() {
    asm volatile (
        "sti;" // Enable interrupts during system calls (Disabled by SYSENTER)

        // EBP contains the user mode ESP and [EBP -4] contains the user mode return address
        "push %%ebp;"
        "push -4(%%ebp);"

        // Push parameters for the system call
        "push %%edi;"
        "push %%esi;"
        "push %%edx;"
        "push %%ecx;"
        "push %%ebx;"

        // Call the system call
        "call *%P0(,%%eax,4);"

        // EDX contains the upper half of the return value, but must contain the return address
        // -> We move the return value to EBX and restore EDX in user mode
        "mov %%edx, %%ebx;"
        "add $20, %%esp;" // Skip parameters
        "pop %%edx;" // Get return address from stack
        "pop %%ecx;" // Get user ESP from stack

        // Return to user mode
        "sysexit;"
        :
        : "i"(systemCalls)
        );
}

}
