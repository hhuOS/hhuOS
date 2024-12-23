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

#ifndef HHUOS_NODE_H
#define HHUOS_NODE_H

#include "lib/util/base/String.h"
#include "lib/util/io/file/File.h"

namespace Filesystem {

/**
 * Represents a node in the filesystem.
 * When a file/folder is requested, the Filesystem-class returns a pointer to an FsNode,
 * that corresponds to the requested file/folder. It can then be used to read/write
 * to the file and get meta-information.
 */
class Node {

public:
    /**
     * Constructor.
     */
    Node() = default;

    /**
     * Copy Constructor.
     */
    Node(const Node &copy) = delete;

    /**
     * Assignment operator.
     */
     Node& operator=(const Node &other) = delete;

    /**
     * Destructor.
     */
    virtual ~Node() = default;

    /**
     * Get the name.
     */
    virtual Util::String getName() = 0;

    /**
     * Get the file type.
     */
    virtual Util::Io::File::Type getType() = 0;

    /**
     * Get the length (in bytes) of the node's data.
     * If the node is a directory, this should always return 0.
     */
    virtual uint64_t getLength() = 0;

    /**
     * Get the node's children.
     */
    virtual Util::Array<Util::String> getChildren() = 0;

    /**
     * Read bytes from the node's data.
     * If (pos + numBytes) is greater than the data's length, END_OF_FILE shall be appended.
     * If pos is greater than the data's length, END_OF_FILE shall be the first and only character,
     * that is written to buf.
     *
     * @param targetBuffer The buffer to write to (Needs to be allocated already!)
     * @param pos The offset
     * @param numBytes The amount of bytes to read
     *
     * @return The amount of actually read bytes
     */
    virtual uint64_t readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) = 0;

    /**
     * Write bytes to the node's data. If the offset points right into the existing data,
     * it shall be overwritten with the new data. If the new data does not fit, the data size shall be increased.
     *
     * @param sourceBuffer The data to write
     * @param pos The offset
     * @param numBytes The amount of bytes to write
     *
     * @return The amount of actually written bytes
     */
    virtual uint64_t writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t length) = 0;

    /**
     * Check if this node is readable without blocking. Regular files are always ready to read.
     * This function is mainly useful for character files (i.e. streams), such as terminals or sockets.
     *
     * @return Whether this node is readable without blocking
     */
    virtual bool isReadyToRead() {
        switch (getType()) {
            case Util::Io::File::REGULAR:
                return true;
            case Util::Io::File::DIRECTORY:
                return false;
            default:
                Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "Node does not implement 'isReadyToRead()'!");
        }
    }

    /**
     * If this nodes represents a device, this function can be used to manipulate this device.
     * The parameters are implementation dependent.
     *
     * @param request The function to be executed on the device
     * @param parameters The parameters for the function
     *
     * @return true, on success
     */
    virtual bool control([[maybe_unused]] uint32_t request, [[maybe_unused]] const Util::Array<uint32_t> &parameters) {
        return false;
    }
};

}

#endif
