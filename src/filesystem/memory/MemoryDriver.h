/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_MEMORYDRIVER_H
#define HHUOS_MEMORYDRIVER_H

#include "filesystem/VirtualDriver.h"
#include "lib/util/base/String.h"
#include "lib/util/io/file/File.h"

namespace Filesystem {

namespace Memory {
class MemoryDirectoryNode;
class MemoryNode;
}
}  // namespace Filesystem

namespace Filesystem::Memory {

class MemoryDriver : public VirtualDriver {

public:
    /**
     * Constructor.
     */
    MemoryDriver();

    /**
     * Copy Constructor.
     */
    MemoryDriver(const MemoryDriver &copy) = delete;

    /**
     * Assignment operator.
     */
    MemoryDriver& operator=(const MemoryDriver &other) = delete;

    /**
     * Destructor.
     */
    ~MemoryDriver() override;

    /**
     * Overriding virtual function from VirtualDriver.
     */
    Node* getNode(const Util::String &path) override;

    /**
     * Overriding virtual function from VirtualDriver.
     */
    bool createNode(const Util::String &path, Util::Io::File::Type type) override;

    /**
     * Overriding virtual function from VirtualDriver.
     */
    bool deleteNode(const Util::String &path) override;

    /**
     * Add an existing virtual node at a specified path.
     *
     * @param path The path
     * @param node The node
     *
     * @return true on success
     */
    bool addNode(const Util::String &path, MemoryNode *node);

private:

    MemoryDirectoryNode *rootNode;

};

}

#endif
