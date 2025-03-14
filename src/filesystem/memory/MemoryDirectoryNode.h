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

#ifndef HHUOS_MEMORYDIRECTORYNODE_H
#define HHUOS_MEMORYDIRECTORYNODE_H

#include <stdint.h>

#include "MemoryNode.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/base/String.h"
#include "lib/util/io/file/File.h"

namespace Filesystem::Memory {

class MemoryDirectoryNode : public MemoryNode {

public:
    /**
     * Constructor.
     */
    explicit MemoryDirectoryNode(const Util::String &name);

    /**
     * Copy Constructor.
     */
    MemoryDirectoryNode(const MemoryDirectoryNode &copy) = delete;

    /**
     * Assignment operator.
     */
    MemoryDirectoryNode& operator=(const MemoryDirectoryNode &other) = delete;

    /**
     * Destructor.
     */
    ~MemoryDirectoryNode() override = default;

    /**
     * Overriding function from Node.
     */
    Util::Io::File::Type getType() override;

    /**
     * Overriding function from Node.
     */
    uint64_t getLength() override;

    /**
     * Overriding function from Node.
     */
    Util::Array<Util::String> getChildren() override;

    /**
     * Overriding function from Node.
     */
    uint64_t readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from Node.
     */
    uint64_t writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) override;

    /**
     * Add a virtual node to this directory.
     *
     * @param node The node
     */
    void addChild(MemoryNode *node);

private:

    /**
     * Get the child of a VirtualNode for its name.
     * CAUTION: May return nullptr, if no child with the specified name is found.
     *
     * @param parent The node
     * @param childName The child's name
     * @return The child (or nullptr on failure)
     */
    MemoryNode* getChildByName(const Util::String &childName);

    Util::ArrayList<MemoryNode*> children;

    friend class MemoryDriver;

};

}

#endif
