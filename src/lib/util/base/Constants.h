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

#ifndef HHUOS_LIB_UTIL_CONSTANTS_H
#define HHUOS_LIB_UTIL_CONSTANTS_H

#include <stddef.h>

namespace Util {

/// The size of a page in bytes (4096 = 4 KiB).
static constexpr size_t PAGESIZE = 0x1000;

/// The maximum size of a user space stack (0x40000 = 256 KiB).
static constexpr size_t MAX_USER_STACK_SIZE = 0x40000;

/// The start address of the user space memory (0x8000000 = 128 MiB).
static constexpr size_t USER_SPACE_MEMORY_START_ADDRESS = 0x20000000;

/// The start address of the user space stack memory (0xf0000000 = 3.75 GiB).
static constexpr size_t USER_SPACE_STACK_MEMORY_START_ADDRESS = 0xf0000000;

/// The start address of each application's main stack.
static constexpr size_t MAIN_STACK_START_ADDRESS = USER_SPACE_STACK_MEMORY_START_ADDRESS;

/// The highest address of the user space memory (0xffffffff + 1 = 4 GiB).
static constexpr size_t MEMORY_END_ADDRESS = 0xffffffff + 1;

}

#endif