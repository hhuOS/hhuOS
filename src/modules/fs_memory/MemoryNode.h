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

#ifndef HHUOS_MEMORYNODE_H
#define HHUOS_MEMORYNODE_H


#include <filesystem/RamFs/VirtualNode.h>

class MemoryNode : public VirtualNode {

protected:
    String cache;

public:
    /**
     * Constructor
     */
    MemoryNode(String name, FsNode::FileType type);

    /**
     * Copy-constructor.
     */
    MemoryNode(const MemoryNode &copy) = delete;

    /**
     * Destructor.
     */
    ~MemoryNode() override = default;

    /**
     * Overriding function from VirtualNode.
     */
    uint64_t getLength() override;

    /**
     * Overriding function from VirtualNode.
     */
    uint64_t readData(char *buf, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from VirtualNode.
     */
    uint64_t writeData(char *buf, uint64_t pos, uint64_t numBytes) override;

    virtual void getValues() = 0;
};


#endif //HHUOS_MEMORYNODE_H
