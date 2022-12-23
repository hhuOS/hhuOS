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
 * The network stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#ifndef HHUOS_IP4PORTADDRESS_H
#define HHUOS_IP4PORTADDRESS_H

#include <cstdint>

#include "lib/util/network/NetworkAddress.h"
#include "lib/util/network/ip4/Ip4Address.h"
#include "lib/util/memory/String.h"

namespace Util::Network::Ip4 {

class Ip4PortAddress : public NetworkAddress {

public:
    /**
     * Default Constructor.
     */
    Ip4PortAddress();

    /**
     * Constructor.
     */
    explicit Ip4PortAddress(uint8_t *buffer);

    /**
     * Constructor.
     */
    explicit Ip4PortAddress(const Util::Memory::String &string);

    /**
     * Constructor.
     */
    Ip4PortAddress(const Ip4Address &address, uint16_t port);

    /**
     * Constructor.
     */
    explicit Ip4PortAddress(const Ip4Address &address);

    /**
     * Constructor.
     */
    explicit Ip4PortAddress(uint16_t port);

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

    void setPort(uint16_t port);

    [[nodiscard]] NetworkAddress* createCopy() const override;

    void setAddress(const Util::Memory::String &string) override;

    [[nodiscard]] Util::Memory::String toString() const override;

    static const uint32_t ADDRESS_LENGTH = Ip4Address::ADDRESS_LENGTH + sizeof(uint16_t);
};

}

#endif
