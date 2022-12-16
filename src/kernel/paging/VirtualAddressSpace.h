/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef __VIRTUALADDRESSSPACE__
#define __VIRTUALADDRESSSPACE__

namespace Util {
namespace Memory {
class HeapMemoryManager;
}  // namespace Memory
}  // namespace Util

namespace Kernel {
class PageDirectory;

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
    explicit VirtualAddressSpace(Util::Memory::HeapMemoryManager &kernelHeapMemoryManager);

    /**
     * Constructor for user address space.
     * The heap always starts at 0x2000.
     */
    explicit VirtualAddressSpace(PageDirectory &basePageDirectory);

    /**
     * Assignment operator.
     */
    VirtualAddressSpace &operator=(const VirtualAddressSpace &other) = delete;

    /**
     * Destructor
     */
    ~VirtualAddressSpace();

    [[nodiscard]] PageDirectory& getPageDirectory() const;

    [[nodiscard]] Util::Memory::HeapMemoryManager& getMemoryManager() const;

    [[nodiscard]] bool isKernelAddressSpace() const;

private:

    PageDirectory *pageDirectory;
    Util::Memory::HeapMemoryManager *memoryManager;

    bool kernelAddressSpace;
};

}

#endif
