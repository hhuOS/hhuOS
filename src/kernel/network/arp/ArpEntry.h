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
 *
 * The UDP/IP stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#ifndef HHUOS_ARPENTRY_H
#define HHUOS_ARPENTRY_H

#include "lib/util/network/ip4/Ip4Address.h"
#include "lib/util/network/MacAddress.h"

namespace Kernel::Network::Arp {

class ArpEntry {

public:
    /**
     * Default Constructor.
     */
    ArpEntry() = default;

    /**
     * Constructor.
     */
    ArpEntry(const Util::Network::Ip4::Ip4Address &protocolAddress, const Util::Network::MacAddress &hardwareAddress);

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

    [[nodiscard]] const Util::Network::Ip4::Ip4Address& getProtocolAddress() const;

    [[nodiscard]] const Util::Network::MacAddress& getHardwareAddress() const;

    void setProtocolAddress(const Util::Network::Ip4::Ip4Address &protocolAddress);

    void setHardwareAddress(const Util::Network::MacAddress &hardwareAddress);

    bool operator!=(const ArpEntry &other) const;

    bool operator==(const ArpEntry &other) const;

private:

    Util::Network::Ip4::Ip4Address protocolAddress{};
    Util::Network::MacAddress hardwareAddress{};
};

}

#endif
