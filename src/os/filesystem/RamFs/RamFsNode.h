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

#ifndef __RamFsNode_include__
#define __RamFsNode_include__

#include <cstdint>
#include "filesystem/FsNode.h"
#include "VirtualNode.h"

/**
 * An implementation of FsNode for the RamFs.
 * It is basically a wrapper-class for VirtualNode.
 *
 * @author Fabian Ruhland
 * @date 2017
 */
class RamFsNode : public FsNode {

public:
    /**
     * Constructor.
     *
     * @param node The virtual node, which is represented by this instance of RamFsNode.
     */
    explicit RamFsNode(VirtualNode *node);

    /**
     * Destructor.
     */
    ~RamFsNode() override = default;

    /**
     * Overriding function from FsNode.
     */
    String getName() override;

    /**
     * Overriding function from FsNode.
     */
    uint8_t getFileType() override;

    /**
     * Overriding function from FsNode.
     */
    uint64_t getLength() override;

    /**
     * Overriding function from FsNode.
     */
    Util::Array<String> getChildren() override;

    /**
     * Overriding function from FsNode.
     */
    uint64_t readData(char *buf, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from FsNode.
     */
    uint64_t writeData(char *buf, uint64_t pos, uint64_t numBytes) override;

private:

    VirtualNode *node;
};

#endif