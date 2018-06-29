/*
 * Copyright (C) 2018  Filip Krakowski, Fabian Ruhland
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <devices/graphics/text/CgaText.h>
#include <lib/libc/printf.h>
#include <kernel/interrupts/BlueScreen.h>
#include "Kernel.h"
#include "KernelSymbols.h"
#include "kernel/Cpu.h"
#include "Bios.h"

Spinlock Kernel::serviceLock;

Util::HashMap<String, KernelService*> Kernel::serviceMap(SERVICE_MAP_SIZE);


void Kernel::registerService(const String &serviceId, KernelService* const &kernelService) {

    serviceLock.acquire();

    serviceMap.put(serviceId, kernelService);

    serviceLock.release();
}

bool Kernel::isServiceRegistered(const String &serviceId) {

    return serviceMap.containsKey(serviceId);
}

void Kernel::panic(InterruptFrame *frame) {

    Cpu::disableInterrupts();

    BlueScreen blueScreen;

    blueScreen.initialize();

    blueScreen.print(*frame);

    Cpu::halt();
}
