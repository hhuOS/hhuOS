/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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
#include "kernel/paging/MemoryLayout.h"
#include "kernel/multiboot/Multiboot.h"
#include "lib/util/collection/ArrayList.h"

namespace Device {

const CopyInformation *SmBios::copyInformation{};
const SmBios::Info *SmBios::smBiosInformation{};

void SmBios::copyTables(const void *multibootInfo, uint8_t *destination, uint32_t maxBytes) {
    auto *copyInfo = reinterpret_cast<CopyInformation*>(destination);
    copyInfo->sourceAddress = 0;
    copyInfo->targetAreaSize = maxBytes;
    copyInfo->copiedBytes = sizeof(CopyInformation);
    copyInfo->success = false;

    auto destinationAddress = Util::Address<uint32_t>(destination + sizeof(CopyInformation));
    Info info{};

    // Search for SMBIOS tables in Multiboot tags
    auto *smBiosTag = findTables(multibootInfo);
    if (smBiosTag != nullptr) {
        auto tableSize = static_cast<uint16_t>(smBiosTag->header.size - 16);
        auto tableAddress = tableSize == 0 ? 0 : reinterpret_cast<uint32_t>(smBiosTag->tables);
        info = { smBiosTag->majorVersion, smBiosTag->minorVersion, tableAddress, tableSize };
    }

    if (info.tableAddress == 0) {
        // Search for entry point the "traditional" way
        auto *entryPoint = searchEntryPoint(0xf0000, 0xfffff);
        auto *entryPoint3 = searchEntryPoint3(0xf0000, 0xfffff);

        if (entryPoint3 != nullptr) {
            info = { entryPoint3->majorVersion, entryPoint3->minorVersion, static_cast<uint32_t>(entryPoint3->tableAddress), entryPoint3->maxTableLength };
        } else if (entryPoint != nullptr) {
            info = { entryPoint->majorVersion, entryPoint->minorVersion, entryPoint->tableAddress, entryPoint->tableLength };
        } else {
            return;
        }
    }

    copyInfo->sourceAddress = reinterpret_cast<uint32_t>(info.tableAddress);
    if (info.tableAddress == 0) {
        return;
    }

    // Copy information struct
    if (copyInfo->copiedBytes + sizeof(Info) > maxBytes) return;
    destinationAddress.copyRange(Util::Address<uint32_t>(&info), sizeof(Info));
    auto *copiedInfo = reinterpret_cast<Info*>(destinationAddress.get());
    destinationAddress = destinationAddress.add(sizeof(Info));
    copyInfo->copiedBytes += sizeof(Info);

    // Copy SMBIOS tables
    if (copyInfo->copiedBytes + copiedInfo->tableLength > maxBytes) return;
    destinationAddress.copyRange(Util::Address(copiedInfo->tableAddress), copiedInfo->tableLength);
    copiedInfo->tableAddress = Kernel::MemoryLayout::PHYSICAL_TO_VIRTUAL(destinationAddress.get());
    copyInfo->copiedBytes += copiedInfo->tableLength;

    copyInfo->success = true;
}

void SmBios::initialize() {
    copyInformation = reinterpret_cast<const CopyInformation*>(&smbios_data);
    if (copyInformation->success) {
        smBiosInformation = reinterpret_cast<const Info*>(&smbios_data + sizeof(CopyInformation));
    }
}

bool SmBios::isAvailable() {
    return copyInformation->success;
}

const CopyInformation& SmBios::getCopyInformation() {
    return *copyInformation;
}

const SmBios::Info &SmBios::getSmBiosInformation() {
    return *smBiosInformation;
}

bool SmBios::hasTable(Util::Hardware::SmBios::HeaderType headerType) {
    auto *currentTable = reinterpret_cast<const Util::Hardware::SmBios::TableHeader*>(smBiosInformation->tableAddress);
    while (currentTable->type != Util::Hardware::SmBios::END_OF_TABLE) {
        if (currentTable->type == headerType) {
            return true;
        }

        currentTable = reinterpret_cast<const Util::Hardware::SmBios::TableHeader*>(reinterpret_cast<uint32_t>(currentTable) + currentTable->calculateFullLength());
    }

    return false;
}

Util::Array<Util::Hardware::SmBios::HeaderType> SmBios::getAvailableTables() {
    if (!isAvailable()) {
        return Util::Array<Util::Hardware::SmBios::HeaderType>(0);
    }

    auto typeList = Util::ArrayList<Util::Hardware::SmBios::HeaderType>();
    auto *currentTable = reinterpret_cast<const Util::Hardware::SmBios::TableHeader*>(smBiosInformation->tableAddress);
    while (currentTable->type != Util::Hardware::SmBios::END_OF_TABLE) {
        typeList.add(currentTable->type);
        currentTable = reinterpret_cast<const Util::Hardware::SmBios::TableHeader*>(reinterpret_cast<uint32_t>(currentTable) + currentTable->calculateFullLength());
    }

    return typeList.toArray();
}

const Kernel::Multiboot::SmBiosTables* SmBios::findTables(const void *multibootInfo) {
    auto currentAddress = reinterpret_cast<uint32_t>(multibootInfo) + sizeof(Kernel::Multiboot::Info);
    auto *currentTag = reinterpret_cast<const Kernel::Multiboot::TagHeader*>(currentAddress);
    while (currentTag->type != Kernel::Multiboot::TERMINATE) {
        if (currentTag->type == Kernel::Multiboot::SMBIOS_TABLES) {
            return reinterpret_cast<const Kernel::Multiboot::SmBiosTables*>(currentTag);
        }

        currentAddress += currentTag->size;
        currentAddress = currentAddress % 8 == 0 ? currentAddress : (currentAddress / 8) * 8 + 8;
        currentTag = reinterpret_cast<const Kernel::Multiboot::TagHeader*>(currentAddress);
    }

    return nullptr;
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