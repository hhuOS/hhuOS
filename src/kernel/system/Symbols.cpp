/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "kernel/paging/MemLayout.h"
#include "System.h"
#include "Symbols.h"

namespace Kernel {

Util::Data::HashMap<Util::Memory::String, Util::Memory::Address<uint32_t>> Symbols::symbolTable(1129);

Util::Data::HashMap<Util::Memory::Address<uint32_t>, char *> Symbols::debugTable(1129);

Multiboot::ElfInfo Symbols::symbolInfo;

bool Symbols::initialized = false;

uint32_t pow(uint32_t base, uint32_t exponent) {

    if (exponent == 0) {
        return 1;
    }

    if (base == 0) {
        return 0;
    }

    uint32_t ret = base;

    for (uint32_t i = 1; i < exponent; i++) {
        ret *= base;
    }

    return ret;
}

uint32_t hextoint(const char *hexString) {

    uint32_t len = Util::Memory::Address<uint32_t>(hexString).stringLength();

    uint32_t ret = 0;

    char current;

    uint32_t num = 0;

    for (uint32_t i = len; i > 0; i--) {

        current = hexString[i - 1];

        if (current > 47 && current < 58) {
            num = current - '0';
        } else if (current > 64 && current < 71) {
            num = current - '7';
        } else if (current > 96 && current < 103) {
            num = current - 'W';
        } else {
            break;
        }

        ret += num * pow(16, len - i);
    }

    return ret;
}

uint32_t Symbols::get(const Util::Memory::String &name) {

    if (symbolTable.containsKey((char *) name)) {

        return (uint32_t) symbolTable.get((char *) name);
    }

    return 0x0;
}

const char *Symbols::get(uint32_t eip) {

    if (!initialized) {

        return NO_INFORMATION;
    }

    while (!debugTable.containsKey(Util::Memory::Address(eip))) {

        eip--;
    }

    return debugTable.get(Util::Memory::Address(eip));
}

void Symbols::load(const Util::File::Elf::Constants::SectionHeader &sectionHeader) {

    uint32_t numEntries = sectionHeader.size / sectionHeader.entrySize;

    auto entry = (Util::File::Elf::Constants::SymbolEntry *) Kernel::MemoryLayout::PHYS2VIRT(sectionHeader.virtualAddress);

    auto stringSection = (Util::File::Elf::Constants::SectionHeader *) (symbolInfo.address +
                                                                                  sectionHeader.link *
                                                                                  symbolInfo.sectionSize);

    char *stringTable = (char *) Kernel::MemoryLayout::PHYS2VIRT(stringSection->virtualAddress);

    for (uint32_t i = 0; i < numEntries; i++, entry++) {

        if (entry->value < Kernel::MemoryLayout::VIRT_KERNEL_START || entry->getSymbolType() == Util::File::Elf::Constants::SymbolType::SECTION) {

            continue;
        }

        char *symbol = &stringTable[entry->nameOffset];

        uint32_t address = entry->value;

        symbolTable.put(symbol, Util::Memory::Address(address));

        debugTable.put(Util::Memory::Address(address), symbol);
    }
}

void Symbols::initialize(const Multiboot::ElfInfo &elfInfo) {

    symbolInfo = elfInfo;

    Util::File::Elf::Constants::SectionHeader *sectionHeader = nullptr;

    for (uint32_t i = 0; i < symbolInfo.sectionCount; i++) {

        sectionHeader = (Util::File::Elf::Constants::SectionHeader *) (symbolInfo.address + i * symbolInfo.sectionSize);

        if (sectionHeader->type == Util::File::Elf::Constants::SectionHeaderType::SYMTAB) {

            load(*sectionHeader);
        }
    }

    initialized = true;
}

void Symbols::copy(const Multiboot::ElfInfo &elfInfo, Util::Memory::Address<uint32_t> &destination) {
    Util::File::Elf::Constants::SectionHeader *sectionHeader = nullptr;
    for (uint32_t i = 0; i < elfInfo.sectionCount; i++) {
        sectionHeader = (Util::File::Elf::Constants::SectionHeader *) (elfInfo.address + i * elfInfo.sectionSize);
        // only copy the sections that are not part of the loaded program
        if ((sectionHeader->virtualAddress & Kernel::MemoryLayout::VIRT_KERNEL_START) == 0) {
            // only copy symbols and strings, discard the rest
            if (sectionHeader->type == Util::File::Elf::Constants::SectionHeaderType::SYMTAB
            || sectionHeader->type == Util::File::Elf::Constants::SectionHeaderType::STRTAB) {
                auto source = Util::Memory::Address<uint32_t>(sectionHeader->virtualAddress, sectionHeader->size);
                destination.copyRange(source, sectionHeader->size);
                sectionHeader->virtualAddress = (elf32_addr) destination.get();
                destination = destination.add(sectionHeader->size);
            } else {
                sectionHeader->virtualAddress = 0;
            }
        }
    }
}

bool Symbols::isInitialized() {

    return initialized;
}

}
