/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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
 *
 * The APIC implementation is based on a bachelor's thesis, written by Christoph Urlacher.
 * The original source code can be found here: https://github.com/ChUrl/hhuOS
 */

#include "SymmetricMultiprocessing.h"
#include "kernel/log/Log.h"
#include "kernel/memory/MemoryLayout.h"
#include "kernel/memory/VirtualAddressSpace.h"
#include "lib/util/base/Constants.h"
#include "device/interrupt/apic/Apic.h"
#include "kernel/service/InterruptService.h"
#include "kernel/service/Service.h"

namespace Device {

volatile bool runningApplicationProcessors[256]{}; // Once an AP is running it sets its corresponding entry to true

[[noreturn]] void applicationProcessorEntry(uint8_t virtualCpuId) {
    runningApplicationProcessors[virtualCpuId] = true; // Mark this AP as running

    while (true) {}

    // Initialize this AP's APIC
    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
    auto &apic = interruptService.getApic();
    apic.initializeCurrentLocalApic();
    apic.enableCurrentErrorHandler();

    while (!interruptService.isParallelComputingAllowed()) {}
}

}