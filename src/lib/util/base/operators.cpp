/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#include <stdint.h>

#include "lib/interface.h"

void* operator new(uint32_t size) {
    return allocateMemory(size);
}

void* operator new[](uint32_t size) {
    return allocateMemory(size);
}

void operator delete(void *pointer) {
    freeMemory(pointer);
}

void operator delete[](void *pointer) {
    freeMemory(pointer);
}

void *operator new([[maybe_unused]] uint32_t size, void *pointer) {
    return pointer;
}

void *operator new[]([[maybe_unused]] uint32_t size, void *pointer) {
    return pointer;
}

void operator delete(void *, void *) {}

void operator delete[](void *, void *) {}

void operator delete(void *pointer, [[maybe_unused]] uint32_t size) {
    freeMemory(pointer);
}

void operator delete[](void *pointer, [[maybe_unused]] uint32_t size) {
    freeMemory(pointer);
}