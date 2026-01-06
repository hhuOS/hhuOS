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

#include "Acpi.h"

#include "util/base/Address.h"
#include "util/base/Constants.h"
#include "interface.h"

namespace Util {
namespace Hardware {
namespace Acpi {

bool Rsdp::verifyChecksum() const {
    const auto *data = reinterpret_cast<const uint8_t*>(this);
    uint8_t sum = 0;
    for (uint32_t i = 0; i < sizeof(Rsdp); ++i) {
        sum += data[i];
    }

    return sum == 0;
}

bool SdtHeader::verifyChecksum() const {
    const auto *data = reinterpret_cast<const uint8_t*>(this);
    uint8_t sum = 0;
    for (uint32_t i = 0; i < length; ++i) {
        sum += data[i];
    }

    return sum == 0;
}

Tables::Tables(const Rsdt &rsdt) : tables(rsdt.getTableCount()) {
    for (size_t i = 0; i < tables.length(); i++) {
        const auto sdtHeaderPhysical = reinterpret_cast<size_t>(rsdt.getTables()[i]);
        const auto sdtPageOffset = sdtHeaderPhysical % PAGESIZE;
        auto *sdtPage = static_cast<uint8_t*>(mapIO(sdtHeaderPhysical, 1));
        auto *sdtHeaderVirtual = reinterpret_cast<SdtHeader*>(sdtPage + sdtPageOffset);

        if (sdtPageOffset + sdtHeaderVirtual->length > PAGESIZE) {
            const auto sizeWithOffset = sdtPageOffset + sdtHeaderVirtual->length;
            const auto pages = (sizeWithOffset + PAGESIZE - 1) / PAGESIZE;

            delete static_cast<const uint8_t*>(sdtPage);
            sdtPage = static_cast<uint8_t*>(mapIO(sdtHeaderPhysical, pages));
            sdtHeaderVirtual = reinterpret_cast<SdtHeader*>(sdtPage + sdtPageOffset);
        }

        tables[i] = sdtHeaderVirtual;
    }
}

Tables::~Tables() {
    for (const auto *table : tables) {
        const auto pageAddress = (reinterpret_cast<uintptr_t>(table) / PAGESIZE) * PAGESIZE;
        delete reinterpret_cast<uint8_t*>(pageAddress);
    }
}

bool Tables::hasTable(const char *signature) const {
    for (const auto *table : tables) {
        if (Address(table->signature).compareRange(Address(signature), sizeof(SdtHeader::signature)) == 0) {
            return true;
        }
    }

    return false;
}

Array<String> Tables::getSignatures() const {
    auto signatures = Array<String>(tables.length());
    for (size_t i = 0; i < tables.length(); i++) {
        signatures[i] = String(reinterpret_cast<const uint8_t*>(tables[i]->signature),
            sizeof(SdtHeader::signature));
    }

    return signatures;
}

const SdtHeader& Tables::operator[](const char *signature) const {
    for (const auto *table : tables) {
        if (Address(table->signature).compareRange(Address(signature), sizeof(SdtHeader::signature)) == 0) {
            return *table;
        }
    }

    Panic::fire(Panic::INVALID_ARGUMENT, "Acpi: Table not found!");
}

const SdtHeader& Tables::operator[](const String &signature) const {
    return operator[](static_cast<const char*>(signature));
}

}
}
}
