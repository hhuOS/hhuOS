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

#ifndef HHUOS_PARALLELMODENODE_H
#define HHUOS_PARALLELMODENODE_H

#include "filesystem/ram/VirtualNode.h"

namespace Parallel {

template<Parallel::LptPort port>
class ParallelDriver;

template<LptPort port>
class ParallelModeNode : public VirtualNode {

private:

    ParallelDriver<port> *parallel = nullptr;

    String cache;

    static const constexpr char *NAME = "mode";

private:

    void refresh();

public:

    /**
     * Constructor.
     */
    explicit ParallelModeNode(ParallelDriver<port> *parallel);

    /**
     * Copy-constructor.
     */
    ParallelModeNode(const ParallelModeNode &copy) = delete;

    /**
     * Destructor.
     */
    ~ParallelModeNode() override = default;

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
ParallelModeNode<port>::ParallelModeNode(ParallelDriver<port> *parallel) : VirtualNode(NAME, FsNode::REGULAR_FILE),
                                                               parallel(parallel) {

}

template<LptPort port>
uint64_t ParallelModeNode<port>::getLength() {
    refresh();

    return cache.length();
}

template<LptPort port>
uint64_t ParallelModeNode<port>::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    refresh();

    uint32_t length = cache.length();

    if (pos + numBytes > length) {
        numBytes = (uint64_t) (length - pos);
    }

    memcpy(buf, (char *) cache + pos, numBytes);

    return numBytes;
}

template<LptPort port>
uint64_t ParallelModeNode<port>::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    char *buffer = new char[numBytes];
    memcpy(buffer, buf, numBytes);

    String mode = buffer;

    if (mode.toLowerCase().beginsWith("spp")) {
        parallel->setMode(SPP);
    } else if (mode.toLowerCase().beginsWith("epp")) {
        parallel->setMode(EPP);
    }

    delete[] buffer;
    return 0;
}

template<LptPort port>
void ParallelModeNode<port>::refresh() {
    switch (parallel->getMode()) {
        case SPP :
            cache = "SPP\n";
            break;
        case EPP :
            cache = "EPP\n";
            break;
    }
}

}

#endif
