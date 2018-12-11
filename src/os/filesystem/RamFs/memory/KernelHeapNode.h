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

#ifndef HHUOS_KERNELHEAPNODE_H
#define HHUOS_KERNELHEAPNODE_H


#include <filesystem/RamFs/VirtualNode.h>
#include <kernel/memory/SystemManagement.h>
#include "MemoryNode.h"

class KernelHeapNode : public MemoryNode {

public:
    KernelHeapNode() : MemoryNode("kernelheap", FsNode::REGULAR_FILE) {}

    /**
     * Copy-constructor.
     */
    KernelHeapNode(const KernelHeapNode &copy) = delete;

    /**
     * Destructor.
     */
    ~KernelHeapNode() override = default;

    void getValues() override {
        uint32_t free = SystemManagement::getKernelHeapManager()->getFreeMemory();
        uint32_t start = SystemManagement::getKernelHeapManager()->getStartAddress();
        uint32_t end = SystemManagement::getKernelHeapManager()->getEndAddress();

        cache = String("KernelHeap\n") +
                String("Start: 0x") + String::valueOf(start, 16) +
                String(" End: 0x") + String::valueOf(end, 16) +
                String(" Free: ") + String::valueOf(free, 10, false) + " Bytes\n";

    }
};


#endif //HHUOS_KERNELHEAPNODE_H
