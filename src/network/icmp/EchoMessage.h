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

#ifndef HHUOS_ECHOMESSAGE_H
#define HHUOS_ECHOMESSAGE_H

#include "IcmpHeader.h"

namespace Network::Icmp {

class EchoMessage {

public:
    /**
     * Default Constructor.
     */
    EchoMessage() = default;

    /**
     * Copy Constructor.
     */
    EchoMessage(const EchoMessage &other) = delete;

    /**
     * Assignment operator.
     */
    EchoMessage &operator=(const EchoMessage &other) = delete;

    /**
     * Destructor.
     */
    ~EchoMessage();

    void read(Util::Stream::InputStream &stream, uint32_t length);

    void write(Util::Stream::OutputStream &stream) const;

    [[nodiscard]] uint16_t getIdentifier() const;

    void setIdentifier(uint16_t identifier);

    [[nodiscard]] uint16_t getSequenceNumber() const;

    void setSequenceNumber(uint16_t sequenceNumber);

    [[nodiscard]] uint8_t *getData() const;

    void setData(uint8_t *data, uint32_t length);

    [[nodiscard]] uint32_t getDataLength() const;

private:

    uint16_t identifier{};
    uint16_t sequenceNumber{};

    uint8_t *data{};
    uint32_t dataLength;
};

}

#endif
