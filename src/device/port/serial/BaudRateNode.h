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

#ifndef HHUOS_BAUDRATENODE_H
#define HHUOS_BAUDRATENODE_H

#include "filesystem/ram/VirtualNode.h"
#include "Common.h"

namespace Serial {

template<ComPort port>
class SerialDriver;

template<ComPort port>
class BaudRateNode : public VirtualNode {

private:

    SerialDriver<port> *serial = nullptr;

    String cache;

    static const constexpr char *NAME = "speed";

private:

    void refresh();

public:

    /**
     * Constructor.
     */
    explicit BaudRateNode(SerialDriver<port> *serial);

    /**
     * Copy-constructor.
     */
    BaudRateNode(const BaudRateNode &copy) = delete;

    /**
     * Destructor.
     */
    ~BaudRateNode() override = default;

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

template<ComPort port>
BaudRateNode<port>::BaudRateNode(SerialDriver<port> *serial) : VirtualNode(NAME, FsNode::REGULAR_FILE), serial(serial) {

}

template<ComPort port>
uint64_t BaudRateNode<port>::getLength() {
    refresh();

    return cache.length();
}

template<ComPort port>
uint64_t BaudRateNode<port>::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    refresh();

    uint32_t length = cache.length();

    if (pos + numBytes > length) {
        numBytes = (uint64_t) (length - pos);
    }

    memcpy(buf, (char *) cache + pos, numBytes);

    return numBytes;
}

template<ComPort port>
uint64_t BaudRateNode<port>::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    char *buffer = new char[numBytes];

    memcpy(buffer, buf, numBytes);

    auto baud = static_cast<uint32_t>(strtoint(buffer));

    if (baud != 0) {
        if (115200 % baud != 0 || baud < 2) {
            // baud rate must be a restless divider of 115200 greater than 1
            delete[] buffer;
            return 0;
        }

        if (baud == 115200) {
            serial->setSpeed(BaudRate::BAUD_115200);
        } else {
            serial->setSpeed(static_cast<BaudRate>(115200 / baud));
        }

        delete[] buffer;
        return numBytes;
    }

    delete[] buffer;
    return 0;
}

template<ComPort port>
void BaudRateNode<port>::refresh() {
    auto speed = (uint16_t) serial->getSpeed();

    cache = String::valueOf(115200 / speed, 10) + "\n";
}

}

#endif