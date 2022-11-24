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

#ifndef HHUOS_ARPENTRY_H
#define HHUOS_ARPENTRY_H

#include "network/ip4/Ip4Address.h"
#include "network/MacAddress.h"

namespace Network::Arp {

class ArpEntry {

public:
    /**
     * Default Constructor.
     */
    ArpEntry() = default;

    /**
     * Constructor.
     */
    ArpEntry(const Ip4::Ip4Address &protocolAddress, const MacAddress &hardwareAddress);

    /**
     * Copy Constructor.
     */
    ArpEntry(const ArpEntry &other) = default;

    /**
     * Assignment operator.
     */
    ArpEntry &operator=(const ArpEntry &other) = default;

    /**
     * Destructor.
     */
    ~ArpEntry() = default;

    [[nodiscard]] const Ip4::Ip4Address& getProtocolAddress() const;

    [[nodiscard]] const MacAddress& getHardwareAddress() const;

    void setProtocolAddress(const Ip4::Ip4Address &protocolAddress);

    void setHardwareAddress(const MacAddress &hardwareAddress);

    bool operator!=(const ArpEntry &other) const;

    bool operator==(const ArpEntry &other) const;

private:

    Ip4::Ip4Address protocolAddress{};
    MacAddress hardwareAddress{};
};

}

#endif
