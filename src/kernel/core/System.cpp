/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#include "System.h"
#include "Symbols.h"
#include "device/cpu/Cpu.h"

extern "C" {
int32_t atexit (void (*func)()) noexcept;
}

int32_t atexit (void (*func)()) noexcept {
    return 0;
}

namespace Kernel {

Async::Spinlock System::serviceLock;

Util::HashMap<Util::String, KernelService *> System::serviceMap(SERVICE_MAP_SIZE);

void System::registerService(const Util::String &serviceId, KernelService *const &kernelService) {

    serviceLock.acquire();

    serviceMap.put(serviceId, kernelService);

    serviceLock.release();
}

bool System::isServiceRegistered(const Util::String &serviceId) {

    return serviceMap.containsKey(serviceId);
}

void System::panic(InterruptFrame *frame) {

    Device::Cpu::disableInterrupts();

    Device::Cpu::halt();
}

}