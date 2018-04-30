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

#ifndef HHUOS_IOMEMORYNODE_H
#define HHUOS_IOMEMORYNODE_H


#include <kernel/memory/SystemManagement.h>
#include "MemoryNode.h"

class IOMemoryNode : public MemoryNode {

public:
    IOMemoryNode() : MemoryNode("iomemory", FsNode::REGULAR_FILE) {}

    /**
     * Copy-constructor.
     */
    IOMemoryNode(const IOMemoryNode &copy) = delete;

    /**
     * Destructor.
     */
    ~IOMemoryNode() override = default;

    void getValues() override {
        uint32_t free = SystemManagement::getInstance()->getIOMemoryManager()->getFreeMemory();
        uint32_t start = SystemManagement::getInstance()->getIOMemoryManager()->getStartAddress();
        uint32_t end = SystemManagement::getInstance()->getIOMemoryManager()->getEndAddress();

        cache = String("IO-Memory\n") +
                String("Start: 0x") + String::valueOf(start, 16) +
                String(" End: 0x") + String::valueOf(end, 16) +
                String(" Free: ") + String::valueOf(free, 10) + " Bytes\n";

    }
};


#endif //HHUOS_IOMEMORYNODE_H
