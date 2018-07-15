/*
 * Copyright (C) 2018  Filip Krakowski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ElfConstants.h"

using namespace ElfConstants;

bool FileHeader::isValid() {
    if (magic[0] != 0x7F ||
        magic[1] != 'E'  ||
        magic[2] != 'L'  ||
        magic[3] != 'F') {
        return false;
    }

    if (architecture != Architecture::BIT_32||
        byteOrder != ByteOrder::LITTLE_ENDIAN     ||
        machine != MachineType::X86) {
        return false;
    }

    return !(type != ElfType::RELOCATABLE && type != ElfType::EXECUTABLE);
}

bool FileHeader::hasProgramEntries() {
    return programHeaderEntries != 0;
}

uint8_t RelocationEntry::getIndex() {
    return (uint8_t) (info >> 8);
}

RelocationType RelocationEntry::getType() {
    return RelocationType(info & 0xFF);
}

SymbolBinding SymbolEntry::getBinding() {
    return SymbolBinding(info >> 4);
}

SymbolType SymbolEntry::getType() {
    return SymbolType(info & 0xF);
}
