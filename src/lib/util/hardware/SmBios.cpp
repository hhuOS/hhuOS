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

#include <stddef.h>

#include "SmBios.h"

#include "collection/ArrayList.h"
#include "math/Math.h"

namespace Util::Hardware {

size_t SmBios::TableHeader::calculateFullLength() const {
    const char *stringTable = reinterpret_cast<const char*>(this) + length;

    // Search the string table for two consecutive null characters, indicating the end of the string table.
    size_t i = 1;
    while (stringTable[i - 1] != 0 || stringTable[i] != 0) {
        i++;
    }

    return length + i + 1;
}

size_t SmBios::TableHeader::calculateStringCount() const {
    const char *stringTable = reinterpret_cast<const char*>(this) + length;

    size_t index = 0;
    size_t strings = 0;
    while (true) {
        // Iterate through the current string, increasing the index until we reach the end of the string.
        while (stringTable[index] != 0) {
            index++;
        }

        // If the next character right after a string is a null character, we have reached the end of the string table.
        if (stringTable[index + 1] == 0) {
            break;
        }

        index++; // Skip null terminator
        strings++; // Increase string count after each string
    }

    return strings;
}

const char* SmBios::TableHeader::getString(const size_t number) const {
    const char *stringTable = reinterpret_cast<const char*>(this) + length;

    size_t index = 0;
    for (size_t i = 1; i < number; i++) {
        while (stringTable[index] != 0) {
            index++;
        }
        index++;
    }

    return &stringTable[index];
}

SmBios::Tables::Tables(const TableHeader &firstTable) : firstTable(&firstTable) {}

const SmBios::TableHeader& SmBios::Tables::operator[](const HeaderType type) const {
    for (const auto &table : *this) {
        if (table.type == type) {
            return table;
        }
    }

    Panic::fire(Panic::INVALID_ARGUMENT, "SmBios: Table not found!");
}

size_t SmBios::Tables::getTableCount() const {
    size_t count = 0;
    for ([[maybe_unused]] const auto &table : *this) {
        count++;
    }

    return count;
}

bool SmBios::Tables::hasTable(const HeaderType type) const {
    for (const auto &table : *this) {
        if (table.type == type) {
            return true;
        }
    }

    return false;
}

Array<SmBios::HeaderType> SmBios::Tables::getTypes() const {
    ArrayList<HeaderType> typeList;
    for (const auto &table : *this) {
        typeList.add(table.type);
    }

    return typeList.toArray();
}

Iterator<const SmBios::TableHeader> SmBios::Tables::begin() const {
    const auto element = IteratorElement<const TableHeader>{ firstTable, 0 };
    return Iterator<const TableHeader>(*this, element);
}

Iterator<const SmBios::TableHeader> SmBios::Tables::end() const {
    constexpr auto element = IteratorElement<const TableHeader>{ nullptr, 0 };
    return Iterator<const TableHeader>(*this, element);
}

IteratorElement<const SmBios::TableHeader>
    SmBios::Tables::next(const IteratorElement<const TableHeader> &element) const
{
    const auto length = element.data->calculateFullLength();
    const auto *nextTable = reinterpret_cast<TableHeader*>(reinterpret_cast<uintptr_t>(element.data) + length);

    if (nextTable->type == END_OF_TABLE) {
        return IteratorElement<const TableHeader>{ nullptr, 0 };
    }

    return IteratorElement<const TableHeader>{ nextTable, element.index + 1 };
}

const char* SmBios::BiosInformation::getVendorName() const {
    return header.getString(vendorString);
}

const char* SmBios::BiosInformation::getVersion() const {
    return header.getString(versionString);
}

const char* SmBios::BiosInformation::getReleaseDate() const {
    return header.getString(releaseDateString);
}

uint32_t SmBios::BiosInformation::calculateRuntimeSize() const {
    return (0x10000 - startAddressSegment) * 16;
}

uint32_t SmBios::BiosInformation::calculateRomSize() const {
    return static_cast<uint32_t>(Math::powInt(65536, romSize + 1));
}

}
