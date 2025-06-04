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

#include "Acpi.h"

#include "lib/util/base/Address.h"
#include "lib/util/base/String.h"
#include "kernel/multiboot/Multiboot.h"
#include "kernel/service/Service.h"
#include "kernel/service/InformationService.h"
#include "kernel/service/MemoryService.h"
#include "kernel/log/Log.h"
#include "lib/util/base/Constants.h"

namespace Device {

Acpi::Acpi() {
    rsdp = findRsdp();
    if (rsdp != nullptr) {
        auto vendor = Util::String(reinterpret_cast<const uint8_t*>(rsdp->oemId), sizeof(Util::Hardware::Acpi::Rsdp::oemId));
        LOG_INFO("ACPI vendor: [%s], ACPI version: [%s]", static_cast<const char*>(vendor.strip()), rsdp->revision == 0 ? "1.0" : ">=2.0");

        auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
        auto rsdtPageOffset = reinterpret_cast<uint32_t>(rsdp->rsdtAddress) % Util::PAGESIZE;
        auto *rsdtPage = memoryService.mapIO(const_cast<void*>(reinterpret_cast<const void*>(rsdp->rsdtAddress)), 1);
        auto *rsdt = reinterpret_cast<Util::Hardware::Acpi::Rsdt*>(reinterpret_cast<uint8_t*>(rsdtPage) + rsdtPageOffset);
        if (rsdtPageOffset + rsdt->header.length > Util::PAGESIZE) {
            auto pages = (rsdtPageOffset + rsdt->header.length) % Util::PAGESIZE == 0 ? (rsdtPageOffset + rsdt->header.length) / Util::PAGESIZE : (rsdtPageOffset + rsdt->header.length) / Util::PAGESIZE + 1;
            delete static_cast<uint8_t*>(rsdtPage);
            rsdtPage = memoryService.mapIO(const_cast<void*>(reinterpret_cast<const void*>(rsdp->rsdtAddress)), pages);
            rsdt = reinterpret_cast<Util::Hardware::Acpi::Rsdt*>(static_cast<uint8_t*>(rsdtPage) + rsdtPageOffset);
        }

        tables = new Util::Hardware::Acpi::Tables(*rsdt);

        Util::String tableString;
        for (const auto &signature : tables->getSignatures()) {
            tableString += signature + " ";
        }

        LOG_INFO("ACPI tables: %s", static_cast<const char*>(tableString));
    }
}

const Util::Hardware::Acpi::Rsdp& Acpi::getRsdp() const {
    return *rsdp;
}

const Util::Hardware::Acpi::Tables& Acpi::getTables() const {
    return *tables;
}

const Util::Hardware::Acpi::Rsdp* Acpi::findRsdp() {
    // Search for RSDP in Multiboot tags
    const Util::Hardware::Acpi::Rsdp *rsdp = nullptr;
    const Util::Hardware::Acpi::Rsdp *xsdp = nullptr;

    const auto &multiboot = Kernel::Service::getService<Kernel::InformationService>().getMultibootInformation();
    if (multiboot.hasTag(Kernel::Multiboot::ACPI_OLD_RSDP)) {
        rsdp = &multiboot.getTag<Kernel::Multiboot::AcpiRsdp>(Kernel::Multiboot::ACPI_OLD_RSDP).rsdp;
    } else if (multiboot.hasTag(Kernel::Multiboot::ACPI_NEW_RSDP)) {
        xsdp = &multiboot.getTag<Kernel::Multiboot::AcpiRsdp>(Kernel::Multiboot::ACPI_NEW_RSDP).rsdp;
    }

    if (rsdp != nullptr) {
        LOG_INFO("Found RSDP in multiboot tags");
        return rsdp;
    } else if (xsdp != nullptr) {
        LOG_INFO("Found XSDP in multiboot tags");
        return xsdp;
    }

    // Search for RSDP the "traditional" way
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds" // GCC complains about the rsdp address being out of bounds, but it is not (it is just located at a very low address)
    auto ebdaStartAddress = *reinterpret_cast<uint16_t*>(0x0000040e) << 4;
#pragma GCC diagnostic pop
    auto rsdpAddress = searchRsdp(ebdaStartAddress, ebdaStartAddress + 1023);
    if (rsdpAddress != nullptr) {
        LOG_INFO("Found RSDP in extended bios area");
        return rsdpAddress;
    }

    rsdpAddress = searchRsdp(0x000e0000, 0x000fffff);
    if (rsdpAddress != nullptr) {
        LOG_INFO("Found RSDP in bios area");
        return rsdpAddress;
    }

    LOG_ERROR("RSDP not found -> ACPI not available");
    return nullptr;
}

const Util::Hardware::Acpi::Rsdp* Acpi::searchRsdp(uint32_t startAddress, uint32_t endAddress) {
    char signature[sizeof(Util::Hardware::Acpi::Rsdp::signature)] = {'R', 'S', 'D', ' ', 'P', 'T', 'R', ' '};
    auto signatureAddress = Util::Address(signature);
    startAddress = Util::Address(startAddress).alignUp(16).get();

    for (uint32_t i = startAddress; i <= endAddress - sizeof(signature); i += 16) {
        auto address = Util::Address(i);
        if (address.compareRange(signatureAddress, sizeof(Util::Hardware::Acpi::Rsdp::signature)) == 0) {
            auto *rsdp = reinterpret_cast<const Util::Hardware::Acpi::Rsdp*>(i);
            if (rsdp->verifyChecksum()) {
                return reinterpret_cast<const Util::Hardware::Acpi::Rsdp*>(i);
            }
        }
    }

    return nullptr;
}

}