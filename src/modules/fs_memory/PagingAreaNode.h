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
#ifndef HHUOS_PAGINGAREANODE_H
#define HHUOS_PAGINGAREANODE_H


#include <kernel/memory/SystemManagement.h>
#include "MemoryNode.h"

class PagingAreaNode : public MemoryNode {

public:
    PagingAreaNode() : MemoryNode("pagingarea", FsNode::REGULAR_FILE) {}

    /**
     * Copy-constructor.
     */
    PagingAreaNode(const PagingAreaNode &copy) = delete;

    /**
     * Destructor.
     */
    ~PagingAreaNode() override = default;

    void getValues() override {
        uint32_t free = SystemManagement::getInstance().getPagingAreaManager()->getFreeMemory();
        uint32_t start = SystemManagement::getInstance().getPagingAreaManager()->getStartAddress();
        uint32_t end = SystemManagement::getInstance().getPagingAreaManager()->getEndAddress();

        cache = String::format("Start: 0x%08x\nEnd: 0x%08x\nFree: %u Bytes\n", start, end, free);
    }
};


#endif //HHUOS_PAGINGAREANODE_H
