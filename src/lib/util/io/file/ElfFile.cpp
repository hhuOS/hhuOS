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

#include "ElfFile.h"

#include <io/stream/FileInputStream.h>

#include "base/Address.h"
#include "base/Panic.h"

namespace Util::Io {

ElfFile::ElfFile(uint8_t *buffer) : deleteBuffer(false), buffer(buffer) {
    parseFileHeader();
}

ElfFile::ElfFile(const File &file) : deleteBuffer(true), buffer(new uint8_t[file.getLength()]) {
    FileInputStream inputStream(file);
    inputStream.read(buffer, 0, file.getLength());

    parseFileHeader();
}

ElfFile::~ElfFile() {
    if (deleteBuffer) {
        delete[] buffer;
    }
}

void ElfFile::loadProgram() const {
    for (int i = 0; i < fileHeader.programHeaderEntries; i++) {
        const auto header = programHeaders[i];

        if (header.type == ProgramHeaderType::LOAD) {
            auto sourceAddress = Address(buffer + header.offset);
            auto targetAddress = Address(header.virtualAddress);

            targetAddress.copyRange(sourceAddress, header.fileSize);
        }
    }
}

uintptr_t ElfFile::getEndAddress() const {
    uintptr_t ret = 0;

    for (int i = 0; i < fileHeader.programHeaderEntries; i++) {
        const auto header = programHeaders[i];

        if (header.type == ProgramHeaderType::LOAD) {
            const auto size = header.virtualAddress + header.memorySize;

            if (size > ret) {
                ret = size;
            }
        }
    }

    return ret;
}

int (*ElfFile::getEntryPoint() const)(int, char**) {
    return reinterpret_cast<int(*)(int, char**)>(fileHeader.entry);
}

const ElfFile::SectionHeader& ElfFile::getSectionHeader(const SectionType headerType) const {
    for (int i = 0; i < fileHeader.sectionHeaderEntries; i++) {
        const auto &header = sectionHeaders[i];

        if (header.type == headerType && i != fileHeader.sectionHeaderStringIndex) {
            return header;
        }
    }

    Util::Panic::fire(Panic::INVALID_ARGUMENT, "ELF: Section header not found!");
}

void ElfFile::parseFileHeader() {
    if (!fileHeader.isValid()) {
        Util::Panic::fire(Panic::INVALID_ARGUMENT, "Elf: Invalid file!");
    }

    const auto &sectionHeaderStringHeader = *reinterpret_cast<SectionHeader*>(
        buffer + fileHeader.sectionHeader + fileHeader.sectionHeaderStringIndex * fileHeader.sectionHeaderEntrySize);
    sectionNames = reinterpret_cast<char*>(buffer + sectionHeaderStringHeader.offset);
    programHeaders = reinterpret_cast<ProgramHeader*>(buffer + fileHeader.programHeader);
    sectionHeaders = reinterpret_cast<SectionHeader*>(buffer + fileHeader.sectionHeader);
}

bool ElfFile::FileHeader::isValid() const {
    if (magic[0] != 0x7F || magic[1] != 'E' || magic[2] != 'L' || magic[3] != 'F') {
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

bool ElfFile::FileHeader::hasProgramEntries() const {
    return programHeaderEntries != 0;
}

size_t ElfFile::RelocationEntry::getSymbolIndex() const {
    return info >> 8;
}

ElfFile::RelocationType ElfFile::RelocationEntry::getType() const {
    return static_cast<RelocationType>(info & 0xff);
}

ElfFile::SymbolBinding ElfFile::SymbolEntry::getSymbolBinding() const {
    return static_cast<SymbolBinding>(info >> 4);
}

ElfFile::SymbolType ElfFile::SymbolEntry::getSymbolType() const {
    return static_cast<SymbolType>(info & 0x0f);
}

}
