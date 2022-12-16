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

#ifndef HHUOS_ICMPDATAGRAM_H
#define HHUOS_ICMPDATAGRAM_H

#include <cstdint>

#include "network/Datagram.h"
#include "IcmpHeader.h"

namespace Network {
namespace Ip4 {
class Ip4Address;
}  // namespace Ip4
}  // namespace Network

namespace Network::Icmp {

class IcmpDatagram : public Datagram {

public:
    /**
     * Default Constructor.
     */
    IcmpDatagram(const uint8_t *buffer, uint16_t length, const Ip4::Ip4Address &remoteAddress, IcmpHeader::Type type, uint8_t code);

    /**
     * Copy Constructor.
     */
    IcmpDatagram(const IcmpDatagram &other) = delete;

    /**
     * Assignment operator.
     */
    IcmpDatagram &operator=(const IcmpDatagram &other) = delete;

    /**
     * Destructor.
     */
    ~IcmpDatagram() override = default;

    [[nodiscard]] IcmpHeader::Type getType() const;

    [[nodiscard]] uint8_t getCode() const;

private:

    IcmpHeader::Type type;
    uint8_t code;
};

}

#endif
