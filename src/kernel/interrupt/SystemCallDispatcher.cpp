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

namespace Kernel {

void(*SystemCallDispatcher::systemCalls[256])();

void SystemCallDispatcher::dispatch() {
    asm volatile (
        "push %%edi;"
        "push %%esi;"
        "push %%edx;"
        "push %%ecx;"
        "push %%ebx;"
        "call *%P0(,%%eax,4);"
        "pop %%ebx;"
        "pop %%ecx;"
        "add $4,%%esp;" // Skip EDX, as it contains the upper 32 bits of the return value
        "pop %%esi;"
        "pop %%edi;"
        "iret;"
        :
        : "i"(systemCalls)
        );
}

}