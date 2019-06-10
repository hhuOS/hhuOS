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

#ifndef HHUOS_SERIALNODE_H
#define HHUOS_SERIALNODE_H

#include "kernel/service/PortService.h"
#include "kernel/core/Kernel.h"
#include "filesystem/ram/VirtualNode.h"
#include "Common.h"

namespace Serial {

template<ComPort port>
class SerialDriver;

/**
 * Implementation of VirtualNode, that writes to and reads from a COM-Port.
 *
 * @author Fabian Ruhland
 * @date 2018
 */
template<ComPort port>
class SerialNode : public VirtualNode {

private:

    SerialDriver<port> *serial = nullptr;

    static const constexpr char *NAME = "com";

public:
    /**
     * Constructor.
     */
    explicit SerialNode(SerialDriver<port> *serial);

    /**
     * Copy-constructor.
     */
    SerialNode(const SerialNode &copy) = delete;

    /**
     * Destructor.
     */
    ~SerialNode() override = default;

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

template <ComPort port>
SerialNode<port>::SerialNode(SerialDriver<port> *serial) : VirtualNode(NAME, FsNode::CHAR_FILE), serial(serial) {

}

template <ComPort port>
uint64_t SerialNode<port>::getLength() {
    return 0;
}

template <ComPort port>
uint64_t SerialNode<port>::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    serial->readData(buf, static_cast<uint32_t>(numBytes));

    // Convert carriage returns to '\n'
    for (uint32_t i = 0; i < numBytes; i++) {
        if (buf[i] == 13) {
            buf[i] = '\n';
        }
    }

    return numBytes;
}

template <ComPort port>
uint64_t SerialNode<port>::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    serial->sendData(buf, static_cast<uint32_t>(numBytes));

    return numBytes;
}

}

#endif
