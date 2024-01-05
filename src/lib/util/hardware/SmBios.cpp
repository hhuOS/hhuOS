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

#include "SmBios.h"

namespace Util::Hardware {

uint16_t SmBios::TableHeader::calculateFullLength() const {
    const char *stringTable = reinterpret_cast<const char*>(this) + length;

    uint16_t i;
    for (i = 1; stringTable[i - 1] != 0 || stringTable[i] != 0; i++) {}

    return length + i + 1;
}

uint8_t SmBios::TableHeader::calculateStringCount() const {
    const char *stringTable = reinterpret_cast<const char*>(this) + length;

    uint8_t index = 0;
    uint8_t i;
    for (i = 1;; i++) {
        while (stringTable[index] != 0) {
            index++;
        }

        if (stringTable[index + 1] == 0) {
            break;
        }

        index++;
    }

    return index == 0 ? 0 : i;
}

const char* SmBios::TableHeader::getString(uint8_t number) const {
    const char *stringTable = reinterpret_cast<const char*>(this) + length;

    uint8_t index = 0;
    for (uint8_t i = 1; i < number; i++) {
        while (stringTable[index] != 0) {
            index++;
        }
        index++;
    }

    return &stringTable[index];
}

}