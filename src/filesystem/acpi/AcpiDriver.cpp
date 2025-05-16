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

#include "AcpiDriver.h"

#include "RsdpNode.h"
#include "filesystem/memory/MemoryDirectoryNode.h"
#include "device/system/Acpi.h"
#include "AcpiTableNode.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"
#include "lib/util/hardware/Acpi.h"
#include "kernel/service/Service.h"
#include "kernel/service/InformationService.h"

namespace Filesystem::Acpi {

AcpiDriver::AcpiDriver() {
    addNode("/", new RsdpNode());
    addNode("/", new Memory::MemoryDirectoryNode("tables"));

    const auto &acpi = Kernel::Service::getService<Kernel::InformationService>().getAcpi();
    for (const auto &tableSignature : acpi.getAvailableTables()) {
        const auto &table = acpi.getTable<Util::Hardware::Acpi::SdtHeader>(static_cast<const char*>(tableSignature));
        addNode("/tables", new AcpiTableNode(table));
    }
}

}