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

#ifndef __VirtualNode_include__
#define __VirtualNode_include__

#include "filesystem/core/FsNode.h"
#include <cstdint>
#include "lib/util/ArrayList.h"

extern "C" {
#include "lib/libc/string.h"
}

/**
 * A virtual file or directory used by RamFsDriver and RamFsNode.
 *
 * @author Fabian Ruhland
 * @date 2017
 */
class VirtualNode {

private:
    String name;
    uint8_t fileType;
    uint64_t length;

    char *data = nullptr;

    Util::ArrayList<VirtualNode*> children;

public:
    /**
     * Default-constructor.
     */
    VirtualNode() = delete;

    /**
     * Constructor.
     *
     * @param name The node's name
     * @param fileType The node's file type
     */
    VirtualNode(const String &name, uint8_t fileType);

    /**
     * Copy-constructor.
     */
    VirtualNode(const VirtualNode &copy) = delete;

    /**
     * Destructor.
     */
    virtual ~VirtualNode();

    /**
     * Get the name.
     */
    virtual String getName();

    /**
     * Get the file type.
     */
    virtual uint8_t getFileType();

    /**
     * Get the data's length.
     */
    virtual uint64_t getLength();

    /**
     * Get the node's children.
     */
    Util::ArrayList<VirtualNode*> &getChildren();

    /**
     * Read bytes from the node's data.
     * END_OF_FILE must not be appended, as this will be done by RamFsNode.
     *
     * @param buf The buffer to write to (Needs to be allocated already!)
     * @param pos The offset
     * @param numBytes The amount of bytes to read
     *
     * @return The amount of actually read bytes
     */
    virtual uint64_t readData(char *buf, uint64_t pos, uint64_t numBytes);

    /**
     * Write bytes to the node's data. If the offset points right into the existing data,
     * it shall be overwritten with the new data. If the new data does not fit, the data size shall be increased.
     *
     * @param buf The data to write
     * @param pos The offset
     * @param numBytes The amount of bytes to write
     *
     * @return The amount of actually written bytes
     */
    virtual uint64_t writeData(char *buf, uint64_t pos, uint64_t numBytes);
};

#endif
