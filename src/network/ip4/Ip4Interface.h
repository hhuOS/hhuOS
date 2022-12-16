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

#ifndef HHUOS_IP4INTERFACE_H
#define HHUOS_IP4INTERFACE_H

#include <cstdint>

#include "Ip4Address.h"
#include "Ip4NetworkMask.h"
#include "lib/util/memory/String.h"

namespace Device {
namespace Network {
class NetworkDevice;
}  // namespace Network
}  // namespace Device

namespace Network::Ip4 {

class Ip4Interface {

public:
    /**
     * Constructor.
     */
    Ip4Interface(const Ip4Address &address, const Ip4Address &networkAddress, const Ip4NetworkMask &networkMask, Device::Network::NetworkDevice &device);

    /**
     * Copy Constructor.
     */
    Ip4Interface(const Ip4Interface &other) = delete;

    /**
     * Assignment operator.
     */
    Ip4Interface &operator=(const Ip4Interface &other) = delete;

    /**
     * Destructor.
     */
    ~Ip4Interface() = default;

    [[nodiscard]] Util::Memory::String getDeviceIdentifier() const;

    void sendPacket(uint8_t *packet, uint32_t length);

    [[nodiscard]] const Ip4Address& getAddress() const;

    bool isTargetOf(const Ip4Address &targetAddress);

    [[nodiscard]] Device::Network::NetworkDevice& getDevice() const;

private:

    Ip4Address address;
    Ip4Address networkAddress;
    Ip4NetworkMask networkMask;

    Device::Network::NetworkDevice &device;
};

}

#endif
