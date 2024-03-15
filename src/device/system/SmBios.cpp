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

#include "asm_interface.h"
#include "lib/util/base/Address.h"
#include "kernel/memory/MemoryLayout.h"
#include "kernel/multiboot/Multiboot.h"
#include "lib/util/collection/ArrayList.h"
#include "kernel/service/InformationService.h"
#include "kernel/service/MemoryService.h"

namespace Device {

SmBios::SmBios() {
    // Search for SMBIOS tables in Multiboot information. Since the Multiboot information is already mapped into the kernel address space,
    // we do not need to worry about physical and virtual addresses.
    // Note: Limine does not follow the Multiboot2 standard, and passes the entry point to the kernel, instead of the whole tables.
    //       In this case, 'tableAddress' will be set to nullptr, and the Multiboot tag will be ignored.
    const auto &multiboot = Kernel::Service::getService<Kernel::InformationService>().getMultibootInformation();
    if (multiboot.hasTag(Kernel::Multiboot::SMBIOS_TABLES)) {
        const auto &tag = multiboot.getTag<Kernel::Multiboot::SmBiosTables>(Kernel::Multiboot::SMBIOS_TABLES);
        auto tableSize = static_cast<uint16_t>(tag.header.size - 16);
        auto tableAddress = tableSize == 0 ? nullptr : reinterpret_cast<const Util::Hardware::SmBios::TableHeader*>(tag.tables);
        smBiosInformation = { tag.majorVersion, tag.minorVersion, tableAddress, tableSize };
    }

    if (smBiosInformation.tableAddress == nullptr) {
        // Search for entry point the "traditional" way
        auto *entryPoint = searchEntryPoint(0xf0000, 0xfffff);
        auto *entryPoint3 = searchEntryPoint3(0xf0000, 0xfffff);

        if (entryPoint != nullptr) {
            smBiosInformation = { entryPoint->majorVersion, entryPoint->minorVersion, reinterpret_cast<const Util::Hardware::SmBios::TableHeader*>(entryPoint->tableAddress), entryPoint->tableLength };
        } else if (entryPoint3 != nullptr) {
            smBiosInformation = { entryPoint3->majorVersion, entryPoint3->minorVersion, reinterpret_cast<const Util::Hardware::SmBios::TableHeader*>(entryPoint3->tableAddress), entryPoint3->maxTableLength };
        }

        // The entry point struct contains a physical address, so we need to map the tables in to the kernel address space
        if (smBiosInformation.tableAddress != nullptr) {
            auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
            auto tablePageOffset = reinterpret_cast<uint32_t>(smBiosInformation.tableAddress) % Kernel::Paging::PAGESIZE;
            auto tablePageCount = (tablePageOffset + smBiosInformation.tableLength) % Kernel::Paging::PAGESIZE == 0 ? (tablePageOffset + smBiosInformation.tableLength) / Kernel::Paging::PAGESIZE : ((tablePageOffset + smBiosInformation.tableLength) / Kernel::Paging::PAGESIZE) + 1;
            auto *tablePage = memoryService.mapIO(const_cast<void*>(reinterpret_cast<const void*>(smBiosInformation.tableAddress)), tablePageCount);
            smBiosInformation.tableAddress = reinterpret_cast<const Util::Hardware::SmBios::TableHeader*>(reinterpret_cast<uint8_t*>(tablePage) + tablePageOffset);
        }
    }
}

const SmBios::Info &SmBios::getSmBiosInformation() const {
    return smBiosInformation;
}

bool SmBios::hasTable(Util::Hardware::SmBios::HeaderType headerType) const {
    auto *currentTable = reinterpret_cast<const Util::Hardware::SmBios::TableHeader*>(smBiosInformation.tableAddress);
    while (currentTable->type != Util::Hardware::SmBios::END_OF_TABLE) {
        if (currentTable->type == headerType) {
            return true;
        }

        currentTable = reinterpret_cast<const Util::Hardware::SmBios::TableHeader*>(reinterpret_cast<uint32_t>(currentTable) + currentTable->calculateFullLength());
    }

    return false;
}

Util::Array<Util::Hardware::SmBios::HeaderType> SmBios::getAvailableTables() const {
    if (smBiosInformation.tableAddress == 0) {
        return Util::Array<Util::Hardware::SmBios::HeaderType>(0);
    }

    auto typeList = Util::ArrayList<Util::Hardware::SmBios::HeaderType>();
    auto *currentTable = reinterpret_cast<const Util::Hardware::SmBios::TableHeader*>(smBiosInformation.tableAddress);
    while (currentTable->type != Util::Hardware::SmBios::END_OF_TABLE) {
        typeList.add(currentTable->type);
        currentTable = reinterpret_cast<const Util::Hardware::SmBios::TableHeader*>(reinterpret_cast<uint32_t>(currentTable) + currentTable->calculateFullLength());
    }

    return typeList.toArray();
}

const SmBios::EntryPoint* SmBios::searchEntryPoint(uint32_t startAddress, uint32_t endAddress) {
    char signature[sizeof(EntryPoint::smSignature)] = {'_', 'S', 'M', '_'};
    auto signatureAddress = Util::Address<uint32_t>(signature);
    startAddress = Util::Address<uint32_t>(startAddress).alignUp(16).get();

    for (uint32_t i = startAddress; i <= endAddress - sizeof(signature); i += 16) {
        auto address = Util::Address<uint32_t>(i);
        if (address.compareRange(signatureAddress, sizeof(EntryPoint::smSignature)) == 0) {
            if (checkEntryPoint(reinterpret_cast<EntryPoint*>(i))) {
                return reinterpret_cast<EntryPoint*>(i);
            }
        }
    }

    return nullptr;
}

bool SmBios::checkEntryPoint(SmBios::EntryPoint *entryPoint) {
    uint32_t sum = 0;
    for (uint32_t i = 0; i < entryPoint->length; i++) {
        sum += reinterpret_cast<uint8_t*>(entryPoint)[i];
    }

    return static_cast<uint8_t>(sum) == 0;
}

const SmBios::EntryPoint3* SmBios::searchEntryPoint3(uint32_t startAddress, uint32_t endAddress) {
    char signature[sizeof(EntryPoint3::smSignature)] = {'_', 'S', 'M', '3', '_'};
    auto signatureAddress = Util::Address<uint32_t>(signature);
    startAddress = Util::Address<uint32_t>(startAddress).alignUp(16).get();

    for (uint32_t i = startAddress; i <= endAddress - sizeof(signature); i += 16) {
        auto address = Util::Address<uint32_t>(i);
        if (address.compareRange(signatureAddress, sizeof(EntryPoint3::smSignature)) == 0) {
            if (checkEntryPoint3(reinterpret_cast<EntryPoint3*>(i))) {
                return reinterpret_cast<EntryPoint3*>(i);
            }
        }
    }

    return nullptr;
}

bool SmBios::checkEntryPoint3(SmBios::EntryPoint3 *entryPoint) {
    uint32_t sum = 0;
    for (uint32_t i = 0; i < entryPoint->length; i++) {
        sum += reinterpret_cast<uint8_t*>(entryPoint)[i];
    }

    return static_cast<uint8_t>(sum) == 0;
}

}