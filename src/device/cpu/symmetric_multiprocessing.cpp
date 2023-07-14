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
 *
 * The APIC implementation is based on a bachelor's thesis, written by Christoph Urlacher.
 * The original source code can be found here: https://github.com/ChUrl/hhuOS
 */

#include "symmetric_multiprocessing.h"

#include "kernel/log/Logger.h"
#include "device/interrupt/apic/Apic.h"
#include "kernel/system/System.h"
#include "kernel/service/InterruptService.h"

namespace Device {

volatile bool runningApplicationProcessors[256]{}; // Once an AP is running it sets its corresponding entry to true

Kernel::Logger log = Kernel::Logger::get("SMP");

[[noreturn]] void applicationProcessorEntry(uint8_t initializedApplicationProcessorsCounter) {
    runningApplicationProcessors[initializedApplicationProcessorsCounter] = true; // Mark this AP as running
    while (true) {}

    // Initialize this AP's APIC
    auto &interruptService = Kernel::System::getService<Kernel::InterruptService>();
    auto &apic = interruptService.getApic();
    apic.initializeCurrentLocalApic();
    apic.enableCurrentErrorHandler();

    while (!interruptService.isParallelComputingAllowed()) {}
}

}