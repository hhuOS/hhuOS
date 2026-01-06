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

#include "InformationService.h"

#include "kernel/multiboot/Multiboot.h"
#include "MemoryService.h"
#include "lib/util/base/Constants.h"
#include "kernel/service/Service.h"
#include "lib/util/io/file/ElfFile.h"

namespace Device {
class Acpi;
class SmBios;
}  // namespace Device

Kernel::InformationService::InformationService(const Kernel::Multiboot *multiboot) : multiboot(multiboot) {
    if (multiboot->hasTag(Kernel::Multiboot::ELF_SYMBOLS)) {
        auto &symbolsTag = multiboot->getTag<Kernel::Multiboot::ElfSymbols>(Kernel::Multiboot::ELF_SYMBOLS);
        for (uint32_t i = 0; i < symbolsTag.entryCount; i++) {
            const auto &header = symbolsTag.sectionHeaders[i];
            if (header.type == Util::Io::ElfFile::SectionType::SYMTAB) {
                symbolTableSize = header.size;
                symbolTable = static_cast<const Util::Io::ElfFile::SymbolEntry*>(mapElfSection(header));
            } else if (header.type == Util::Io::ElfFile::SectionType::STRTAB && i != symbolsTag.stringSectionIndex) {
                stringTable = static_cast<const char*>(mapElfSection(header));
            }
        }
    }
}

void Kernel::InformationService::setAcpi(const Device::Acpi *acpi) {
    InformationService::acpi = acpi;
}

void Kernel::InformationService::setSmBios(const Device::SmBios *smBios) {
    InformationService::smBios = smBios;
}

const Kernel::Multiboot& Kernel::InformationService::getMultibootInformation() const {
    return *multiboot;
}

const Device::Acpi& Kernel::InformationService::getAcpi() const {
    return *acpi;
}

const Device::SmBios& Kernel::InformationService::getSmBios() const {
    return *smBios;
}

const char* Kernel::InformationService::getSymbolName(uint32_t symbolAddress) {
    for (uint32_t i = 0; i < symbolTableSize / sizeof(Util::Io::ElfFile::SymbolEntry); i++) {
        const auto &symbol = *(symbolTable + i);
        if (symbol.value == symbolAddress && symbol.getSymbolType() == Util::Io::ElfFile::SymbolType::FUNC) {
                return stringTable + symbol.nameOffset;
        }
    }

    return nullptr;
}

void *Kernel::InformationService::mapElfSection(const Util::Io::ElfFile::SectionHeader &sectionHeader) {
    // Beware: 'sectionHeader.virtualAddress' refers to a physical address in this case, due to the bootloader using an identity mapping
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
    auto pageOffset = sectionHeader.virtualAddress % Util::PAGESIZE;
    auto pageCount = (pageOffset + sectionHeader.size) % Util::PAGESIZE == 0 ? (pageOffset + sectionHeader.size) / Util::PAGESIZE : ((pageOffset + sectionHeader.size) / Util::PAGESIZE) + 1;
    auto mappedSection = memoryService.mapIO(reinterpret_cast<void*>(sectionHeader.virtualAddress), pageCount);

    return reinterpret_cast<uint8_t*>(mappedSection) + pageOffset;
}
