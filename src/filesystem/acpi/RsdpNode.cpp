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

#include "RsdpNode.h"

#include <stdint.h>

#include "device/system/Acpi.h"
#include "lib/util/hardware/Acpi.h"
#include "kernel/service/Service.h"
#include "kernel/service/InformationService.h"
#include "lib/util/base/String.h"

namespace Filesystem::Acpi {

RsdpNode::RsdpNode() : Memory::BufferNode("rsdp", reinterpret_cast<const uint8_t*>(&Kernel::Service::getService<Kernel::InformationService>().getAcpi().getRsdp()), sizeof(Util::Hardware::Acpi::Rsdp)) {}

}