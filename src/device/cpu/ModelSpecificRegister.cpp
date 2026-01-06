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

#include "ModelSpecificRegister.h"

namespace Device {

ModelSpecificRegister::ModelSpecificRegister(uint32_t address) : address(address) {}

uint64_t ModelSpecificRegister::readQuadWord() const {
    uint32_t low;
    uint32_t high;

    // rdmsr writes read value to eax/edx from register specified in ecx; it has no operands
    asm volatile (
            "rdmsr"
            : "=a"(low), "=d"(high)
            : "c"(address)
            );

    return low | (static_cast<uint64_t>(high) << 32);
}

void ModelSpecificRegister::writeQuadWord(uint64_t value) const {
    const uint32_t low = value & 0xFFFFFFFF;
    const uint32_t high = value >> 32;

    // wrmsr writes values from eax/edx to register specified in ecx; it has no operands
    asm volatile (
            "wrmsr"
            :
            : "a"(low), "d"(high), "c"(address)
            );

}

}