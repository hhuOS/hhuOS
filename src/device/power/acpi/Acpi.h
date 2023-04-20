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

#ifndef HHUOS_ACPI_H
#define HHUOS_ACPI_H

#include <cstdint>

#include "lib/util/collection/Array.h"
#include "lib/util/base/Address.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/base/Exception.h"
#include "lib/util/hardware/Acpi.h"

namespace Util {
class String;
}  // namespace Util
struct CopyInformation;

namespace Device {

class Acpi {

public:

    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    Acpi() = delete;

    /**
     * Copy Constructor.
     */
    Acpi(const Acpi &other) = delete;

    /**
     * Assignment operator.
     */
    Acpi &operator=(const Acpi &other) = delete;

    /**
     * Destructor.
     * Deleted, as this class has only static members.
     */
    ~Acpi() = delete;

    static void copyTables(const void *multibootInfo, uint8_t *destination, uint32_t maxBytes);

    static void initialize();

    static bool isAvailable();

    static const CopyInformation& getCopyInformation();

    static const Util::Hardware::Acpi::Rsdp& getRsdp();

    static bool hasTable(const char *signature);

    static Util::Array<Util::String> getAvailableTables();

    template<typename T>
    static const T& getTable(const char *signature);

    template<typename T>
    static Util::Array<const T*> getMadtStructures(Util::Hardware::Acpi::ApicStructureType type);

private:

    static Util::Hardware::Acpi::Rsdp* findRsdp(const void *multibootInfo);

    static Util::Hardware::Acpi::Rsdp* searchRsdp(uint32_t startAddress, uint32_t endAddress);

    static bool checkRsdp(Util::Hardware::Acpi::Rsdp *rsdp);

    static bool checkSdt(Util::Hardware::Acpi::SdtHeader *sdtHeader);

    static const CopyInformation *copyInformation;
    static const Util::Hardware::Acpi::Rsdp *rsdp;
    static const Util::Hardware::Acpi::SdtHeader **tables;
    static uint32_t numTables;
};

template<typename T>
const T& Acpi::getTable(const char *signature) {
    for (uint32_t i = 0; i < numTables; i++) {
        if (Util::Address<uint32_t>(tables[i]->signature).compareRange(Util::Address<uint32_t>(signature), sizeof(Util::Hardware::Acpi::SdtHeader::signature)) == 0) {
            return *reinterpret_cast<const T*>(tables[i]);
        }
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Acpi: Table not found!");
}

template<typename T>
Util::Array<const T*> Acpi::getMadtStructures(Util::Hardware::Acpi::ApicStructureType type) {
    auto structures = Util::ArrayList<const T*>();

    const auto &madt = getTable<Util::Hardware::Acpi::Madt>("APIC");
    const auto *madtEndAddress = reinterpret_cast<const uint8_t*>(&madt) + madt.header.length;

    const auto *pos = reinterpret_cast<const uint8_t*>(&madt.apicStructure);
    const Util::Hardware::Acpi::ApicStructureHeader *header;
    while (pos < madtEndAddress) {
        header = reinterpret_cast<const Util::Hardware::Acpi::ApicStructureHeader*>(pos);

        if (header->length == 0) {
            // If this happens there is a bug in this function o_O
            Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Header length must not be 0!");
        }

        if (header->type == type) {
            structures.add(reinterpret_cast<const T*>(header));
        }

        pos += header->length;
    }

    return structures.toArray();
}

}

#endif
