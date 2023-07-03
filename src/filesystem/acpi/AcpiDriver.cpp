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

#include "AcpiDriver.h"

#include "RsdpNode.h"
#include "filesystem/memory/MemoryDirectoryNode.h"
#include "device/power/acpi/Acpi.h"
#include "AcpiTableNode.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"
#include "lib/util/hardware/Acpi.h"

namespace Filesystem::Acpi {

AcpiDriver::AcpiDriver() {
    addNode("/", new RsdpNode());
    addNode("/", new Memory::MemoryDirectoryNode("tables"));

    for (const auto &tableSignature : Device::Acpi::getAvailableTables()) {
        const auto &table = Device::Acpi::getTable<Util::Hardware::Acpi::SdtHeader>(static_cast<const char*>(tableSignature));
        addNode("/tables", new AcpiTableNode(table));
    }
}

}