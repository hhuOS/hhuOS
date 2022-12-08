/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/memory/Address.h"
#include "lib/util/Exception.h"
#include "File.h"

namespace Util::File::Elf {

bool FileHeader::isValid() {
    if (magic[0] != 0x7F ||
        magic[1] != 'E' ||
        magic[2] != 'L' ||
        magic[3] != 'F') {
        return false;
    }

    if (architecture != Architecture::BIT_32) {
        return false;
    }

    if (byteOrder != ByteOrder::LITTLE_END) {
        return false;
    }

    return machine == MachineType::X86;

}

bool FileHeader::hasProgramEntries() const {
    return programHeaderEntries != 0;
}

uint32_t RelocationEntry::getSymbolIndex() const {
    return (uint32_t) (info >> 8U);
}

RelocationType RelocationEntry::getType() const {
    return RelocationType(info & 0xFFU);
}

SymbolBinding SymbolEntry::getSymbolBinding() const {
    return SymbolBinding(info >> 4U);
}

SymbolType SymbolEntry::getSymbolType() const {
    return SymbolType(info & 0x0FU);
}

File::File(uint8_t *buffer) : buffer(buffer), fileHeader(*reinterpret_cast<FileHeader*>(buffer)) {
    if (!fileHeader.isValid()) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Elf: Invalid file!");
    }

    const auto &sectionHeaderStringHeader = *reinterpret_cast<SectionHeader*>(buffer + fileHeader.sectionHeader + fileHeader.sectionHeaderStringIndex * fileHeader.sectionHeaderEntrySize);
    sectionNames = reinterpret_cast<char*>(buffer + sectionHeaderStringHeader.offset);
    programHeaders = reinterpret_cast<ProgramHeader*>(buffer + fileHeader.programHeader);
    sectionHeaders = reinterpret_cast<SectionHeader*>(buffer + fileHeader.sectionHeader);
}

File::~File() {
    delete buffer;
}

uint32_t File::getEndAddress() {
    uint32_t ret = 0;

    for(int i = 0; i < fileHeader.programHeaderEntries; i++) {
        auto header = programHeaders[i];

        if(header.type == ProgramHeaderType::LOAD) {
            const auto size = header.virtualAddress + header.memorySize;

            if(size > ret) {
                ret = size;
            }
        }
    }

    return ret;
}

void File::loadProgram() {
    for(int i = 0; i < fileHeader.programHeaderEntries; i++) {
        auto header = programHeaders[i];

        if(header.type == ProgramHeaderType::LOAD) {
            auto sourceAddress = Util::Memory::Address<uint32_t>(buffer + header.offset);
            auto targetAddress = Util::Memory::Address<uint32_t>(header.virtualAddress);

            targetAddress.copyRange(sourceAddress, header.fileSize);
        }
    }
}

}