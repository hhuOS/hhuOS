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

#include "Acpi.h"

#include "kernel/memory/MemoryLayout.h"
#include "asm_interface.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/String.h"
#include "kernel/multiboot/Multiboot.h"
#include "kernel/service/Service.h"
#include "kernel/service/InformationService.h"
#include "kernel/memory/Paging.h"
#include "kernel/service/MemoryService.h"

namespace Device {

Acpi::Acpi() {
    rsdp = findRsdp();
    if (rsdp != nullptr) {
        auto *rsdt = const_cast<Util::Hardware::Acpi::Rsdt*>(reinterpret_cast<const Util::Hardware::Acpi::Rsdt*>(mapSdt(reinterpret_cast<Util::Hardware::Acpi::SdtHeader*>(rsdp->rsdtAddress))));

        auto numTables = (rsdt->header.length - sizeof(Util::Hardware::Acpi::SdtHeader)) / sizeof(uint32_t);
        for (uint32_t i = 0; i < numTables; i++) {
            rsdt->tables[i] = mapSdt(rsdt->tables[i]);
        }

        Acpi::rsdt = rsdt;
    }
}

const Util::Hardware::Acpi::Rsdp& Acpi::getRsdp() const {
    return *rsdp;
}

bool Acpi::hasTable(const char *signature) const {
    if (rsdt == nullptr) {
        return false;
    }

    auto numTables = (rsdt->header.length - sizeof(Util::Hardware::Acpi::SdtHeader)) / sizeof(uint32_t);

    for (uint32_t i = 0; i < numTables; i++) {
        if (Util::Address<uint32_t>(rsdt->tables[i]->signature).compareRange(Util::Address<uint32_t>(signature), sizeof(Util::Hardware::Acpi::SdtHeader::signature)) == 0) {
            return true;
        }
    }

    return false;
}

Util::Array<Util::String> Acpi::getAvailableTables() const {
    if (rsdt == nullptr) {
        Util::Array<Util::String>(0);
    }

    auto numTables = (rsdt->header.length - sizeof(Util::Hardware::Acpi::SdtHeader)) / sizeof(uint32_t);
    Util::Array<Util::String> signatures(numTables);

    for (uint32_t i = 0; i < numTables; i++) {
        signatures[i] = Util::String(reinterpret_cast<const uint8_t*>(rsdt->tables[i]->signature), sizeof(Util::Hardware::Acpi::SdtHeader::signature));
    }

    return signatures;
}

const Util::Hardware::Acpi::SdtHeader* Acpi::mapSdt(const Util::Hardware::Acpi::SdtHeader *sdtHeaderPhysical) {
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
    auto sdtPageOffset = reinterpret_cast<uint32_t>(sdtHeaderPhysical) % Kernel::Paging::PAGESIZE;
    auto *sdtPage = memoryService.mapIO(const_cast<void*>(reinterpret_cast<const void*>(sdtHeaderPhysical)), 1);
    auto *sdtHeaderVirtual = reinterpret_cast<Util::Hardware::Acpi::SdtHeader*>(reinterpret_cast<uint8_t*>(sdtPage) + sdtPageOffset);

    if ((sdtPageOffset + sdtHeaderVirtual->length) > Kernel::Paging::PAGESIZE) {
        delete static_cast<uint8_t*>(sdtPage);
        auto pages = (sdtPageOffset + sdtHeaderVirtual->length) % Kernel::Paging::PAGESIZE == 0 ? (sdtPageOffset + sdtHeaderVirtual->length) / Kernel::Paging::PAGESIZE : ((sdtPageOffset + sdtHeaderVirtual->length) / Kernel::Paging::PAGESIZE) + 1;
        sdtPage = memoryService.mapIO(const_cast<void*>(reinterpret_cast<const void*>(sdtHeaderPhysical)), pages);
        sdtHeaderVirtual = reinterpret_cast<Util::Hardware::Acpi::SdtHeader*>(reinterpret_cast<uint8_t*>(sdtPage) + sdtPageOffset);
    }

    return sdtHeaderVirtual;
}

const Util::Hardware::Acpi::Rsdp* Acpi::findRsdp() {
    // Search for RSDP in Multiboot tags
    const Util::Hardware::Acpi::Rsdp *oldRsdp = nullptr;
    const Util::Hardware::Acpi::Rsdp *newRsdp = nullptr;

    const auto &multiboot = Kernel::Service::getService<Kernel::InformationService>().getMultibootInformation();
    if (multiboot.hasTag(Kernel::Multiboot::ACPI_OLD_RSDP)) {
        oldRsdp = &multiboot.getTag<Kernel::Multiboot::AcpiRsdp>(Kernel::Multiboot::ACPI_OLD_RSDP).rsdp;
    } else if (multiboot.hasTag(Kernel::Multiboot::ACPI_NEW_RSDP)) {
        newRsdp = &multiboot.getTag<Kernel::Multiboot::AcpiRsdp>(Kernel::Multiboot::ACPI_NEW_RSDP).rsdp;
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
        return rsdpAddress;
    }

    return searchRsdp(0x000e0000, 0x000fffff);
}

const Util::Hardware::Acpi::Rsdp* Acpi::searchRsdp(uint32_t startAddress, uint32_t endAddress) {
    char signature[sizeof(Util::Hardware::Acpi::Rsdp::signature)] = {'R', 'S', 'D', ' ', 'P', 'T', 'R', ' '};
    auto signatureAddress = Util::Address<uint32_t>(signature);
    startAddress = Util::Address<uint32_t>(startAddress).alignUp(16).get();

    for (uint32_t i = startAddress; i <= endAddress - sizeof(signature); i += 16) {
        auto address = Util::Address<uint32_t>(i);
        if (address.compareRange(signatureAddress, sizeof(Util::Hardware::Acpi::Rsdp::signature)) == 0) {
            if (checkRsdp(reinterpret_cast<const Util::Hardware::Acpi::Rsdp*>(i))) {
                return reinterpret_cast<const Util::Hardware::Acpi::Rsdp*>(i);
            }
        }
    }

    return nullptr;
}

bool Acpi::checkRsdp(const Util::Hardware::Acpi::Rsdp *rsdp) {
    uint32_t sum = 0;
    for (uint32_t i = 0; i < sizeof(Util::Hardware::Acpi::Rsdp); i++) {
        sum += reinterpret_cast<const uint8_t*>(rsdp)[i];
    }

    return static_cast<uint8_t>(sum) == 0;
}

bool Acpi::checkSdt(const Util::Hardware::Acpi::SdtHeader *sdtHeader) {
    uint32_t sum = 0;
    for (uint32_t i = 0; i < sdtHeader->length; i++) {
        sum += reinterpret_cast<const uint8_t*>(sdtHeader)[i];
    }

    return static_cast<uint8_t>(sum) == 0;
}

}