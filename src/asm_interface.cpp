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

#include <cstdint>

#include "kernel/multiboot/Multiboot.h"
#include "kernel/memory/Paging.h"
#include "kernel/service/InterruptService.h"
#include "device/cpu/Cpu.h"
#include "GatesOfHell.h"
#include "device/system/Acpi.h"
#include "kernel/log/Log.h"
#include "kernel/process/ThreadState.h"
#include "kernel/service/SchedulerService.h"
#include "kernel/memory/GlobalDescriptorTable.h"
#include "device/system/SmBios.h"

// Import functions
extern "C" {
void _fini();
}

// Export functions
extern "C" {
void set_tss_stack_entry(uint32_t);
void set_scheduler_init();
bool is_scheduler_initialized();
void release_scheduler_lock();
int32_t atexit (void (*func)()) noexcept;
}

void set_tss_stack_entry(uint32_t esp0) {}

void set_scheduler_init() {
    Kernel::Service::getService<Kernel::SchedulerService>().setSchedulerInit();
}

bool is_scheduler_initialized() {
    return Kernel::Service::getService<Kernel::SchedulerService>().isSchedulerInitialized();
}

void release_scheduler_lock() {
    Kernel::Service::getService<Kernel::SchedulerService>().unlockScheduler();
}

int32_t atexit (void (*func)()) noexcept {
    return 0;
}