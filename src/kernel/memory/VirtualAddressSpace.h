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

#ifndef __VIRTUALADDRESSSPACE__
#define __VIRTUALADDRESSSPACE__

#include <stdint.h>
#include <lib/util/async/Spinlock.h>

#include "Paging.h"

namespace Util {

class HeapMemoryManager;

}  // namespace Util

namespace Kernel {

/**
 * VirtualAddressSpace - represents a virtual address space with corresponding page directory
 * and memory managers.
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date HHU, 2018
 */
class VirtualAddressSpace {

public:
    /**
     * Constructor for the kernel address space.
     */
    explicit VirtualAddressSpace(Paging::Table *physicalPageDirectory, Paging::Table *virtualPageDirectory, Util::HeapMemoryManager &kernelHeapMemoryManager);

    /**
     * Constructor for user address space.
     * The heap always starts at 0x2000.
     */
    explicit VirtualAddressSpace();

    /**
     * Copy constructor.
     */
    VirtualAddressSpace(const VirtualAddressSpace &copy) = delete;

    /**
     * Assignment operator.
     */
    VirtualAddressSpace &operator=(const VirtualAddressSpace &other) = delete;

    /**
     * Destructor
     */
    ~VirtualAddressSpace();

    void* getPhysicalAddress(void *virtualAddress) const;

    void map(const void *physicalAddress, const void *virtualAddress, uint16_t flags, bool abortIfLocked = false);

    void* unmap(const void *virtualAddress);

    Util::HeapMemoryManager& getMemoryManager() const;

    const Paging::Table& getPageDirectoryPhysical() const;

    bool isKernelAddressSpace() const;

private:

    bool kernelAddressSpace;
    Paging::Table *physicalPageDirectory;
    Paging::Table *virtualPageDirectory;
    Util::Async::Spinlock pageDirectoryLock;
    Util::HeapMemoryManager &memoryManager;
};

}

#endif
