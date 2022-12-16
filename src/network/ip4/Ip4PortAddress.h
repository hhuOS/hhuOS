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

#ifndef HHUOS_IP4PORTADDRESS_H
#define HHUOS_IP4PORTADDRESS_H

#include <cstdint>

#include "network/NetworkAddress.h"
#include "lib/util/memory/Address.h"
#include "Ip4Address.h"
#include "lib/util/memory/String.h"

namespace Network::Ip4 {

class Ip4PortAddress : public NetworkAddress {

public:
    /**
     * Default Constructor.
     */
    Ip4PortAddress(const Ip4Address &address, uint16_t port);

    /**
     * Copy Constructor.
     */
    Ip4PortAddress(const Ip4PortAddress &other) = default;

    /**
     * Assignment operator.
     */
    Ip4PortAddress &operator=(const Ip4PortAddress &other) = default;

    /**
     * Destructor.
     */
    ~Ip4PortAddress() override = default;

    [[nodiscard]] Ip4Address getIp4Address() const;

    [[nodiscard]] uint16_t getPort() const;

    [[nodiscard]] NetworkAddress* createCopy() const override;

    void setAddress(const Util::Memory::String &string) override;

    [[nodiscard]] Util::Memory::String toString() const override;

private:

    Util::Memory::Address<uint32_t> bufferAddress;
};

}

#endif
