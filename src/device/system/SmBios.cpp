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

#include "SmBios.h"

#include "lib/util/base/Address.h"
#include "kernel/multiboot/Multiboot.h"
#include "lib/util/collection/ArrayList.h"
#include "kernel/service/InformationService.h"
#include "kernel/service/MemoryService.h"
#include "kernel/log/Log.h"
#include "kernel/service/Service.h"
#include "lib/util/base/Constants.h"

namespace Device {

SmBios::SmBios() {
    // Search for SMBIOS tables in Multiboot information. Since the Multiboot information is already mapped into the kernel address space,
    // we do not need to worry about physical and virtual addresses.
    // Note: Limine does not follow the Multiboot2 standard, and passes the entry point to the kernel, instead of the whole tables.
    //       In this case, 'tableAddress' will be set to nullptr, and the Multiboot tag will be ignored.
    const auto &multiboot = Kernel::Service::getService<Kernel::InformationService>().getMultibootInformation();
    if (multiboot.hasTag(Kernel::Multiboot::SMBIOS_TABLES)) {
        LOG_INFO("Found SMBIOS tables in multiboot tags");
        const auto &tag = multiboot.getTag<Kernel::Multiboot::SmBiosTables>(Kernel::Multiboot::SMBIOS_TABLES);
        auto tableSize = static_cast<uint16_t>(tag.header.size - 16);
        auto tableAddress = tableSize == 0 ? nullptr : reinterpret_cast<const Util::Hardware::SmBios::TableHeader*>(tag.tables);
        smBiosInformation = { tag.majorVersion, tag.minorVersion, tableAddress, tableSize };

        if (smBiosInformation.tableAddress == nullptr) {
            LOG_ERROR("Invalid multiboot SMBIOS tag -> Falling back to manual search");
        } else {
            LOG_INFO("SMBIOS version: [%u.%u]", smBiosInformation.majorVersion, smBiosInformation.minorVersion);
        }
    }

    if (smBiosInformation.tableAddress == nullptr) {
        // Search for entry point the "traditional" way
        auto *entryPoint = searchEntryPoint(0xf0000, 0xfffff);
        auto *entryPoint3 = searchEntryPoint3(0xf0000, 0xfffff);

        if (entryPoint != nullptr) {
            LOG_INFO("Found old entry point in BIOS area");
            smBiosInformation = { entryPoint->majorVersion, entryPoint->minorVersion, reinterpret_cast<const Util::Hardware::SmBios::TableHeader*>(entryPoint->tableAddress), entryPoint->tableLength };
        } else if (entryPoint3 != nullptr) {
            LOG_INFO("Found new entry point in BIOS area");
            smBiosInformation = { entryPoint3->majorVersion, entryPoint3->minorVersion, reinterpret_cast<const Util::Hardware::SmBios::TableHeader*>(entryPoint3->tableAddress), entryPoint3->maxTableLength };
        }

        // The entry point struct contains a physical address, so we need to map the tables in to the kernel address space
        if (smBiosInformation.tableAddress != nullptr) {
            LOG_INFO("SMBIOS version: [%u.%u]", smBiosInformation.majorVersion, smBiosInformation.minorVersion);
            auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
            auto tablePageOffset = reinterpret_cast<uint32_t>(smBiosInformation.tableAddress) % Util::PAGESIZE;
            auto tablePageCount = (tablePageOffset + smBiosInformation.tableLength) % Util::PAGESIZE == 0 ? (tablePageOffset + smBiosInformation.tableLength) / Util::PAGESIZE : ((tablePageOffset + smBiosInformation.tableLength) / Util::PAGESIZE) + 1;
            auto *tablePage = memoryService.mapIO(const_cast<void*>(reinterpret_cast<const void*>(smBiosInformation.tableAddress)), tablePageCount);
            smBiosInformation.tableAddress = reinterpret_cast<const Util::Hardware::SmBios::TableHeader*>(reinterpret_cast<uint8_t*>(tablePage) + tablePageOffset);
        } else {
            LOG_ERROR("SMBIOS not available");
        }
    }

    if (smBiosInformation.tableAddress != nullptr) {
        tables = new Util::Hardware::SmBios::Tables(*smBiosInformation.tableAddress);
    }
}

SmBios::~SmBios() {
    delete tables;
}

const SmBios::Info &SmBios::getSmBiosInformation() const {
    return smBiosInformation;
}

const Util::Hardware::SmBios::Tables& SmBios::getTables() const {
    return *tables;
}

const SmBios::EntryPoint* SmBios::searchEntryPoint(uint32_t startAddress, uint32_t endAddress) {
    char signature[sizeof(EntryPoint::smSignature)] = {'_', 'S', 'M', '_'};
    auto signatureAddress = Util::Address(signature);
    startAddress = Util::Address(startAddress).alignUp(16).get();

    for (uint32_t i = startAddress; i <= endAddress - sizeof(signature); i += 16) {
        auto address = Util::Address(i);
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
    auto signatureAddress = Util::Address(signature);
    startAddress = Util::Address(startAddress).alignUp(16).get();

    for (uint32_t i = startAddress; i <= endAddress - sizeof(signature); i += 16) {
        auto address = Util::Address(i);
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