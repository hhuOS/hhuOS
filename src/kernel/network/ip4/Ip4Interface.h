/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_IP4INTERFACE_H
#define HHUOS_IP4INTERFACE_H

#include "lib/util/network/ip4/Ip4Address.h"
#include "lib/util/network/ip4/Ip4SubnetAddress.h"

namespace Util {
class String;
}  // namespace Util

namespace Device {
namespace Network {
class NetworkDevice;
}  // namespace Network
}  // namespace Device

namespace Kernel::Network::Ip4 {

class Ip4Interface {

public:
    /**
     * Default Constructor.
     */
    Ip4Interface() = default;

    /**
     * Constructor.
     */
    Ip4Interface(const Util::Network::Ip4::Ip4SubnetAddress &address, Device::Network::NetworkDevice &device);

    /**
     * Copy Constructor.
     */
    Ip4Interface(const Ip4Interface &other) = default;

    /**
     * Assignment operator.
     */
    Ip4Interface &operator=(const Ip4Interface &other) = default;

    /**
     * Destructor.
     */
    ~Ip4Interface() = default;

    bool operator==(const Ip4Interface &other);

    bool operator!=(const Ip4Interface &other);

    [[nodiscard]] const Util::String& getDeviceIdentifier() const;

    [[nodiscard]] const Util::Network::Ip4::Ip4SubnetAddress& getSubnetAddress() const;

    [[nodiscard]] Util::Network::Ip4::Ip4Address getIp4Address() const;

    [[nodiscard]] Device::Network::NetworkDevice& getDevice() const;

    [[nodiscard]] bool isTargetOf(const Util::Network::Ip4::Ip4Address &targetAddress) const;

private:

    Util::Network::Ip4::Ip4SubnetAddress address{};
    Device::Network::NetworkDevice *device{};
};

}

#endif
