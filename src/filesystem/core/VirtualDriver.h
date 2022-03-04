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

#ifndef __FsVirtualDriver_include__
#define __FsVirtualDriver_include__

#include "Driver.h"

namespace Filesystem {

/**
 * Interface class for virtual (completely in-memory) filesystem.
 */
class VirtualDriver : public Driver {

public:
    /**
     * Constructor.
     */
    VirtualDriver() = default;

    /**
     * Copy constructor.
     */
    VirtualDriver(const VirtualDriver &copy) = delete;

    /**
     * Assignment operator.
     */
    VirtualDriver& operator=(const VirtualDriver &other) = delete;

    /**
     * Destructor.
     */
    ~VirtualDriver() override = default;

    /**
     * Get an FsNode, representing a file or directory that a given path points to.
     *
     * @param path The path.
     *
     * @return The FsNode (or nulltpr on failure)
     */
    Node* getNode(const Util::Memory::String &path) override = 0;

    /**
     * Create a new empty file or directory at a given path.
     * The parent-directory of the new file must exist beforehand.
     *
     * @param path The path
     *
     * @return true on success
     */
    bool createNode(const Util::Memory::String &path, Util::File::Type type) override = 0;

    /**
     * Delete an existing file or directory at a given path.
     * The file must be a regular file or an empty folder (a leaf in the filesystem tree).
     *
     * @param path The path.
     *
     * @return true on success
     */
    bool deleteNode(const Util::Memory::String &path) override = 0;
};

}

#endif