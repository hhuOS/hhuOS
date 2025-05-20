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

#ifndef HHUOS_ACPI_H
#define HHUOS_ACPI_H

#include <stdint.h>

#include "lib/util/collection/Array.h"
#include "lib/util/base/Address.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/base/Panic.h"
#include "lib/util/hardware/Acpi.h"

namespace Util {
class String;
}  // namespace Util

namespace Device {

class Acpi {

public:
    /**
     * Constructor.
     */
    Acpi();

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
     */
    ~Acpi() = default;

    [[nodiscard]] const Util::Hardware::Acpi::Rsdp& getRsdp() const;

    [[nodiscard]] bool hasTable(const char *signature) const;

    [[nodiscard]] Util::Array<Util::String> getAvailableTables() const;

    template<typename T>
    [[nodiscard]] const T& getTable(const char *signature) const;

    template<typename T>
    [[nodiscard]] Util::Array<const T*> getMadtStructures(Util::Hardware::Acpi::ApicStructureType type) const;

private:

    static const Util::Hardware::Acpi::Rsdp* findRsdp();

    static const Util::Hardware::Acpi::Rsdp* searchRsdp(uint32_t startAddress, uint32_t endAddress);

    static bool checkRsdp(const Util::Hardware::Acpi::Rsdp *rsdp);

    static bool checkSdt(const Util::Hardware::Acpi::SdtHeader *sdtHeader);

    const Util::Hardware::Acpi::SdtHeader* mapSdt(const Util::Hardware::Acpi::SdtHeader *sdtHeaderPhysical);

    const Util::Hardware::Acpi::Rsdp *rsdp = nullptr;
    const Util::Hardware::Acpi::Rsdt *rsdt = nullptr;
};

template<typename T>
const T& Acpi::getTable(const char *signature) const {
    auto numTables = (rsdt->header.length - sizeof(Util::Hardware::Acpi::SdtHeader)) / sizeof(uint32_t);

    for (uint32_t i = 0; i < numTables; i++) {
        if (Util::Address(rsdt->tables[i]->signature).compareRange(Util::Address(signature), sizeof(Util::Hardware::Acpi::SdtHeader::signature)) == 0) {
            return *reinterpret_cast<const T*>(rsdt->tables[i]);
        }
    }

    Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "Acpi: Table not found!");
}

template<typename T>
Util::Array<const T*> Acpi::getMadtStructures(Util::Hardware::Acpi::ApicStructureType type) const {
    auto structures = Util::ArrayList<const T*>();

    if (!hasTable("APIC")) {
        return structures.toArray();
    }

    const auto &madt = getTable<Util::Hardware::Acpi::Madt>("APIC");
    const auto *madtEndAddress = reinterpret_cast<const uint8_t*>(&madt) + madt.header.length;

    const auto *pos = reinterpret_cast<const uint8_t*>(&madt.apicStructure);
    const Util::Hardware::Acpi::ApicStructureHeader *header;
    while (pos < madtEndAddress) {
        header = reinterpret_cast<const Util::Hardware::Acpi::ApicStructureHeader*>(pos);

        if (header->length == 0) {
            // If this happens there is a bug in this function o_O
            Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Header length must not be 0!");
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
