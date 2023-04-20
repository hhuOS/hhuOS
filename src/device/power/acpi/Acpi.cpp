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

#include "Acpi.h"

#include "kernel/paging/MemoryLayout.h"
#include "asm_interface.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/String.h"
#include "kernel/multiboot/Multiboot.h"

namespace Device {

const CopyInformation *Acpi::copyInformation{};
const Util::Hardware::Acpi::Rsdp *Acpi::rsdp{};
const Util::Hardware::Acpi::SdtHeader **Acpi::tables{};
uint32_t Acpi::numTables{};

void Acpi::copyTables(const void *multibootInfo, uint8_t *destination, uint32_t maxBytes) {
    auto *copyInfo = reinterpret_cast<CopyInformation*>(destination);
    copyInfo->sourceAddress = 0;
    copyInfo->targetAreaSize = maxBytes;
    copyInfo->copiedBytes = sizeof(CopyInformation);
    copyInfo->success = false;

    auto destinationAddress = Util::Address<uint32_t>(destination + sizeof(CopyInformation));

    auto *rsdp = findRsdp(multibootInfo);
    copyInfo->sourceAddress = reinterpret_cast<uint32_t>(rsdp);
    if (rsdp == nullptr) {
        return;
    }

    // Copy RSDP
    if (copyInfo->copiedBytes + sizeof(Util::Hardware::Acpi::Rsdp) > maxBytes) return;
    destinationAddress.copyRange(Util::Address<uint32_t>(rsdp), sizeof(Util::Hardware::Acpi::Rsdp));
    rsdp = reinterpret_cast<Util::Hardware::Acpi::Rsdp*>(destinationAddress.get());
    destinationAddress = destinationAddress.add(sizeof(Util::Hardware::Acpi::Rsdp));
    copyInfo->copiedBytes += sizeof(Util::Hardware::Acpi::Rsdp);

    // Check and copy RSDT
    auto *rsdt = reinterpret_cast<Util::Hardware::Acpi::SdtHeader*>(rsdp->rsdtAddress);
    if (!checkSdt(rsdt)) {
        rsdp->rsdtAddress = 0;
        return;
    }

    auto originalRsdtAddress = rsdp->rsdtAddress;
    auto copiedRsdtAddress = destinationAddress.get();

    if (copyInfo->copiedBytes + rsdt->length > maxBytes) return;
    rsdp->rsdtAddress = Kernel::MemoryLayout::PHYSICAL_TO_VIRTUAL(copiedRsdtAddress);
    destinationAddress.copyRange(Util::Address<uint32_t>(rsdt), rsdt->length);
    destinationAddress = destinationAddress.add(rsdt->length);
    copyInfo->copiedBytes += rsdt->length;

    // Copy all tables, pointed to by the RSDT
    auto **originalEntries = reinterpret_cast<Util::Hardware::Acpi::SdtHeader**>(originalRsdtAddress + sizeof(Util::Hardware::Acpi::SdtHeader));
    auto *copiedEntries = reinterpret_cast<uint32_t*>(copiedRsdtAddress + sizeof(Util::Hardware::Acpi::SdtHeader));
    uint32_t numEntries = (rsdt->length - sizeof(Util::Hardware::Acpi::SdtHeader)) / sizeof(uint32_t);

    for (uint32_t i = 0; i < numEntries; i++) {
        auto *sdt = originalEntries[i];
        if (checkSdt(sdt)) {
            if (copyInfo->copiedBytes + sdt->length > maxBytes) return;
            destinationAddress.copyRange(Util::Address<uint32_t>(sdt), sdt->length);
            copiedEntries[i] = Kernel::MemoryLayout::PHYSICAL_TO_VIRTUAL(destinationAddress.get());
            destinationAddress = destinationAddress.add(sdt->length);
            copyInfo->copiedBytes += sdt->length;
        }
    }

    copyInfo->success = true;
}

Util::Hardware::Acpi::Rsdp* Acpi::findRsdp(const void *multibootInfo) {
    // Search for RSDP in Multiboot tags
    Util::Hardware::Acpi::Rsdp *oldRsdp = nullptr;
    Util::Hardware::Acpi::Rsdp *newRsdp = nullptr;

    auto currentAddress = reinterpret_cast<uint32_t>(multibootInfo) + sizeof(Kernel::Multiboot::Info);
    auto *currentTag = reinterpret_cast<const Kernel::Multiboot::TagHeader*>(currentAddress);
    while (currentTag->type != Kernel::Multiboot::TERMINATE) {
        if (currentTag->type == Kernel::Multiboot::ACPI_OLD_RSDP) {
            oldRsdp = const_cast<Util::Hardware::Acpi::Rsdp*>(&reinterpret_cast<const Kernel::Multiboot::AcpiRsdp*>(currentTag)->rsdp);
        } else if (currentTag->type == Kernel::Multiboot::ACPI_NEW_RSDP) {
            newRsdp = const_cast<Util::Hardware::Acpi::Rsdp*>(&reinterpret_cast<const Kernel::Multiboot::AcpiRsdp*>(currentTag)->rsdp);
        }

        currentAddress += currentTag->size;
        currentAddress = currentAddress % 8 == 0 ? currentAddress : (currentAddress / 8) * 8 + 8;
        currentTag = reinterpret_cast<const Kernel::Multiboot::TagHeader*>(currentAddress);
    }

    if (oldRsdp != nullptr) {
        return oldRsdp;
    } else if (newRsdp != nullptr) {
        return newRsdp;
    }

    // Search for RSDP the "traditional" way
    auto ebdaStartAddress = *reinterpret_cast<uint16_t*>(0x0000040e) << 4;
    auto rsdpAddress = searchRsdp(ebdaStartAddress, ebdaStartAddress + 1023);
    if (rsdpAddress != nullptr) {
        return reinterpret_cast<Util::Hardware::Acpi::Rsdp*>(rsdpAddress);
    }

    return reinterpret_cast<Util::Hardware::Acpi::Rsdp*>(searchRsdp(0x000e0000, 0x000fffff));
}

Util::Hardware::Acpi::Rsdp* Acpi::searchRsdp(uint32_t startAddress, uint32_t endAddress) {
    char signature[sizeof(Util::Hardware::Acpi::Rsdp::signature)] = {'R', 'S', 'D', ' ', 'P', 'T', 'R', ' '};
    auto signatureAddress = Util::Address<uint32_t>(signature);
    startAddress = Util::Address<uint32_t>(startAddress).alignUp(16).get();

    for (uint32_t i = startAddress; i <= endAddress - sizeof(signature); i += 16) {
        auto address = Util::Address<uint32_t>(i);
        if (address.compareRange(signatureAddress, sizeof(Util::Hardware::Acpi::Rsdp::signature)) == 0) {
            if (checkRsdp(reinterpret_cast<Util::Hardware::Acpi::Rsdp*>(i))) {
                return reinterpret_cast<Util::Hardware::Acpi::Rsdp*>(i);
            }
        }
    }

    return nullptr;
}

bool Acpi::checkRsdp(Util::Hardware::Acpi::Rsdp *rsdp) {
    uint32_t sum = 0;
    for (uint32_t i = 0; i < sizeof(Util::Hardware::Acpi::Rsdp); i++) {
        sum += reinterpret_cast<uint8_t*>(rsdp)[i];
    }

    return static_cast<uint8_t>(sum) == 0;
}

bool Acpi::checkSdt(Util::Hardware::Acpi::SdtHeader *sdtHeader) {
    uint32_t sum = 0;
    for (uint32_t i = 0; i < sdtHeader->length; i++) {
        sum += reinterpret_cast<uint8_t*>(sdtHeader)[i];
    }

    return static_cast<uint8_t>(sum) == 0;
}

void Acpi::initialize() {
    copyInformation = reinterpret_cast<const CopyInformation*>(&acpi_data);
    if (copyInformation->success) {
        rsdp = reinterpret_cast<const Util::Hardware::Acpi::Rsdp*>(&acpi_data + sizeof(CopyInformation));
        tables = reinterpret_cast<const Util::Hardware::Acpi::SdtHeader**>(rsdp->rsdtAddress + sizeof(Util::Hardware::Acpi::SdtHeader));
        numTables = (reinterpret_cast<Util::Hardware::Acpi::SdtHeader*>(rsdp->rsdtAddress)->length - sizeof(Util::Hardware::Acpi::SdtHeader)) / sizeof(uint32_t);
    } else {
        rsdp = nullptr;
        tables = nullptr;
        numTables = 0;
    }
}

bool Acpi::isAvailable() {
    return copyInformation->success;
}

const CopyInformation &Acpi::getCopyInformation() {
    return *copyInformation;
}

const Util::Hardware::Acpi::Rsdp& Acpi::getRsdp() {
    return *rsdp;
}

bool Acpi::hasTable(const char *signature) {
    for (uint32_t i = 0; i < numTables; i++) {
        if (Util::Address<uint32_t>(tables[i]->signature).compareRange(Util::Address<uint32_t>(signature), sizeof(Util::Hardware::Acpi::SdtHeader::signature)) == 0) {
            return true;
        }
    }

    return false;
}

Util::Array<Util::String> Acpi::getAvailableTables() {
    Util::Array<Util::String> signatures(numTables);
    for (uint32_t i = 0; i < numTables; i++) {
        signatures[i] = Util::String(reinterpret_cast<const uint8_t*>(tables[i]->signature), sizeof(Util::Hardware::Acpi::SdtHeader::signature));
    }

    return signatures;
}

}