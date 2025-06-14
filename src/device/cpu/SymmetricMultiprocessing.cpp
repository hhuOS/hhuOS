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
 *
 * The APIC implementation is based on a bachelor's thesis, written by Christoph Urlacher.
 * The original source code can be found here: https://github.com/ChUrl/hhuOS
 */

#include "SymmetricMultiprocessing.h"

#include "async/Thread.h"
#include "kernel/log/Log.h"
#include "kernel/memory/MemoryLayout.h"
#include "kernel/memory/VirtualAddressSpace.h"
#include "lib/util/base/Constants.h"
#include "device/interrupt/apic/Apic.h"
#include "kernel/service/InterruptService.h"
#include "kernel/service/Service.h"
#include "kernel/service/TimeService.h"
#include "math/Random.h"

namespace Device {

volatile bool runningApplicationProcessors[256]{}; // Once an AP is running it sets its corresponding entry to true

[[noreturn]] void applicationProcessorEntry(uint8_t virtualCpuId) {
    // Initialize this AP's APIC
    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
    auto &apic = interruptService.getApic();
    apic.initializeCurrentLocalApic();
    apic.enableCurrentErrorHandler();

    // Mark this AP as running
    runningApplicationProcessors[virtualCpuId] = true;

    while (true) {}
}

}
