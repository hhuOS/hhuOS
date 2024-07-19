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

#include <cstdint>

#ifndef HHUOS_CONSTANTS_H
#define HHUOS_CONSTANTS_H

namespace Util {

static const constexpr uint32_t PAGESIZE = 0x1000;
static const constexpr uint32_t USER_SPACE_MEMORY_START_ADDRESS = 0x8000000;
static const constexpr uint32_t MAIN_STACK_START_ADDRESS = 0xffff0000;

}

#endif