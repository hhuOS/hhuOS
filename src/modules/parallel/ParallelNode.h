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

#ifndef HHUOS_PARALLELNODE_H
#define HHUOS_PARALLELNODE_H

#include <kernel/Kernel.h>
#include "filesystem/RamFs/VirtualNode.h"
#include "ParallelDriver.h"

namespace Parallel {

/**
 * Implementation of VirtualNode, that writes to an LPT-Port.
 *
 * @author Fabian Ruhland
 * @date 2018
 */
template<LptPort port>
class ParallelNode : public VirtualNode {

private:

    ParallelDriver<port> *parallel = nullptr;

    static const constexpr char *NAME = "lpt";

public:

    /**
     * Constructor.
     */
    explicit ParallelNode(ParallelDriver<port>  *parallel);

    /**
     * Copy-constructor.
     */
    ParallelNode(const ParallelNode &copy) = delete;

    /**
     * Destructor.
     */
    ~ParallelNode() override = default;

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

template<LptPort port>
ParallelNode<port>::ParallelNode(ParallelDriver<port> *parallel) : VirtualNode(NAME, FsNode::BLOCK_FILE), parallel(parallel) {

}

template<LptPort port>
uint64_t ParallelNode<port>::getLength() {
    return 0;
}

template<LptPort port>
uint64_t ParallelNode<port>::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    parallel->readData(buf, static_cast<uint32_t>(numBytes));

    return numBytes;
}

template<LptPort port>
uint64_t ParallelNode<port>::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    parallel->sendData(buf, static_cast<uint32_t>(numBytes));

    return numBytes;
}

}

#endif
