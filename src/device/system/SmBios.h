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

#ifndef HHUOS_SMBIOS_H
#define HHUOS_SMBIOS_H

#include <cstdint>

#include "lib/util/collection/Array.h"
#include "lib/util/hardware/SmBios.h"
#include "lib/util/base/Exception.h"

namespace Device {

class SmBios {

public:

    struct Info {
        uint8_t majorVersion;
        uint8_t minorVersion;
        const Util::Hardware::SmBios::TableHeader *tableAddress;
        uint32_t tableLength;
    };

    /**
     * Constructor.
     */
    SmBios();

    /**
     * Copy Constructor.
     */
    SmBios(const SmBios &other) = delete;

    /**
     * Assignment operator.
     */
    SmBios &operator=(const SmBios &other) = delete;

    /**
     * Destructor.
     */
    ~SmBios() = default;

    [[nodiscard]] const Info& getSmBiosInformation() const;

    [[nodiscard]] bool hasTable(Util::Hardware::SmBios::HeaderType headerType) const;

    [[nodiscard]] Util::Array<Util::Hardware::SmBios::HeaderType> getAvailableTables() const;

    template<typename T>
    [[nodiscard]] const T& getTable(Util::Hardware::SmBios::HeaderType headerType) const;

private:

    struct EntryPoint {
        char smSignature[4];
        uint8_t smChecksum;
        uint8_t length;
        uint8_t majorVersion;
        uint8_t minorVersion;
        uint16_t maxStructureSize;
        uint8_t entryPointRevision;
        char formattedArea[5];
        char dmiSignature[5];
        uint8_t dmiChecksum;
        uint16_t tableLength;
        uint32_t tableAddress;
        uint16_t numStructures;
        uint8_t bcdRevision;
    } __attribute__ ((packed));

    struct EntryPoint3 {
        char smSignature[5];
        uint8_t smChecksum;
        uint8_t length;
        uint8_t majorVersion;
        uint8_t minorVersion;
        uint8_t docRevision;
        uint8_t entryPointRevision;
        uint8_t reserved;
        uint32_t maxTableLength;
        uint64_t tableAddress;
    } __attribute__ ((packed));

    static const EntryPoint* searchEntryPoint(uint32_t startAddress, uint32_t endAddress);

    static bool checkEntryPoint(EntryPoint *entryPoint);

    static const EntryPoint3* searchEntryPoint3(uint32_t startAddress, uint32_t endAddress);

    static bool checkEntryPoint3(EntryPoint3 *entryPoint);

    Info smBiosInformation{};
};

template<typename T>
const T& SmBios::getTable(Util::Hardware::SmBios::HeaderType headerType) const {
    auto *currentTable = reinterpret_cast<const Util::Hardware::SmBios::TableHeader*>(smBiosInformation.tableAddress);
    while (currentTable->type != Util::Hardware::SmBios::END_OF_TABLE) {
        if (currentTable->type == headerType) {
            return *reinterpret_cast<const T*>(currentTable);
        }

        currentTable = reinterpret_cast<const Util::Hardware::SmBios::TableHeader*>(reinterpret_cast<uint32_t>(currentTable) + currentTable->calculateFullLength());
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "SmBios: Table not found!");
}

}

#endif
