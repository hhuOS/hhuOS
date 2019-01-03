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

#ifndef __StorageNode_include__
#define __StorageNode_include__

#include "filesystem/RamFs/VirtualNode.h"
#include "devices/storage/devices/StorageDevice.h"
#include <cstdint>

/**
 * Implementation of VirtualNode, that allows bytewise reading from and writing to a StorageDevice.
 *
 * @author Fabian Ruhland
 * @date 2017
 */
class StorageNode : public VirtualNode {

private:
    StorageDevice *disk;

public:
    /**
     * Default-constructor.
     */
    StorageNode() = delete;

    /**
     * Constructor.
     * @param disk The storage device
     */
    explicit StorageNode(StorageDevice *disk);

    /**
     * Copy-constructor.
     */
    StorageNode(const StorageNode &copy) = delete;

    /**
     * Destructor.
     */
    ~StorageNode() override = default;

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
};

#endif