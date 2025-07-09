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

#include "base/Address.h"
#include "base/Constants.h"
#include "lib/interface.h"

namespace Util::Hardware::Acpi {

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

size_t Rsdt::getTableCount() const {
    return (header.length - sizeof(SdtHeader)) / sizeof(SdtHeader*);
}

Tables::Tables(const Rsdt &rsdt) : tables(rsdt.getTableCount()) {
    for (size_t i = 0; i < tables.length(); i++) {
        const auto *sdtHeaderPhysical = rsdt.tables[i];
        const auto sdtPageOffset = reinterpret_cast<uint32_t>(sdtHeaderPhysical) % PAGESIZE;
        auto *sdtPage = mapIO(const_cast<void*>(reinterpret_cast<const void*>(sdtHeaderPhysical)), 1);
        auto *sdtHeaderVirtual = reinterpret_cast<SdtHeader*>(static_cast<uint8_t*>(sdtPage) + sdtPageOffset);

        if (sdtPageOffset + sdtHeaderVirtual->length > PAGESIZE) {
            const auto pages = (sdtPageOffset + sdtHeaderVirtual->length) % PAGESIZE == 0 ?
                (sdtPageOffset + sdtHeaderVirtual->length) / PAGESIZE :
                (sdtPageOffset + sdtHeaderVirtual->length) / PAGESIZE + 1;

            delete static_cast<const uint8_t*>(sdtPage);
            sdtPage = mapIO(const_cast<void*>(reinterpret_cast<const void*>(sdtHeaderPhysical)), pages);
            sdtHeaderVirtual = reinterpret_cast<SdtHeader*>(static_cast<uint8_t*>(sdtPage) + sdtPageOffset);
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

size_t Tables::getTableCount() const {
    return tables.length();
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

const SdtHeader& Tables::operator[](const size_t index) const {
    return *tables[index];
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

Iterator<SdtHeader> Tables::begin() const {
    const auto element = IteratorElement<SdtHeader>{ tables.length() == 0 ? nullptr : tables[0], 0 };
    return Iterator<SdtHeader>(*this, element);
}

Iterator<SdtHeader> Tables::end() const {
    const auto length = tables.length();
    const auto element = IteratorElement<SdtHeader>{ length == 0 ? nullptr : tables[length], length };

    return Iterator<SdtHeader>(*this, element);
}

IteratorElement<SdtHeader> Tables::next(const IteratorElement<SdtHeader> &element) const {
    return IteratorElement<SdtHeader>{ tables[element.index + 1], element.index + 1 };
}

}
