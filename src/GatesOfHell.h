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

#ifndef __KernelEntry_include__
#define __KernelEntry_include__

#include "device/cpu/GlobalDescriptorTable.h"
#include "lib/util/base/FreeListMemoryManager.h"
#include "kernel/multiboot/Multiboot.h"
#include "kernel/paging/Paging.h"
#include "kernel/memory/PagingAreaManager.h"

namespace Kernel {
class Logger;
}  // namespace Kernel

/**
 * Represents the entry point for the operating system.
 */
class GatesOfHell {

public:
    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    GatesOfHell() = delete;

    /**
     * Copy Constructor.
     */
    GatesOfHell(const GatesOfHell &other) = delete;

    /**
     * Assignment operator.
     */
    GatesOfHell &operator=(const GatesOfHell &other) = delete;

    /**
     * Destructor.
     * Deleted, as this class has only static members.
     */
    ~GatesOfHell() = delete;

    /**
     * Entry point for the operating system.
     * This method is invoked by the main() method, after boot strapping process is finished an paging is initializeAvailableControllers.
     */
    [[noreturn]] static void enter(uint32_t multibootMagic, const Kernel::Multiboot &multiboot);

    static uint32_t createInitialMapping(Kernel::Paging::Table &pageDirectory, Kernel::Paging::Table *pageTableMemory, uint32_t physicalStartAddress, uint32_t virtualStartAddress, uint32_t pageCount);

    static Util::HeapMemoryManager& getKernelHeap();

private:

    static Kernel::Logger log;

    static Device::GlobalDescriptorTable gdt;

    static Device::GlobalDescriptorTable::TaskStateSegment tss;

    static Util::HeapMemoryManager *kernelHeap;
};


#endif
