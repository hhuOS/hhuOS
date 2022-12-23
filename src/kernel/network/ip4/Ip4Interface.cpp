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

#include "Ip4Interface.h"

#include "device/network/NetworkDevice.h"
#include "lib/util/network/NetworkAddress.h"
#include "lib/util/network/ip4/Ip4Address.h"
#include "lib/util/network/ip4/Ip4NetworkMask.h"

namespace Kernel::Network::Ip4 {

Ip4Interface::Ip4Interface(const Util::Network::Ip4::Ip4Address &address, const Util::Network::Ip4::Ip4Address &networkAddress,
                           const Util::Network::Ip4::Ip4NetworkMask &networkMask, Device::Network::NetworkDevice &device) :
        address(address), networkAddress(networkAddress), networkMask(networkMask), device(device) {}

Util::Memory::String Ip4Interface::getDeviceIdentifier() const {
    return device.getIdentifier();
}

void Ip4Interface::sendPacket(uint8_t *packet, uint32_t length) {
    device.sendPacket(packet, length);
}

const Util::Network::Ip4::Ip4Address& Ip4Interface::getAddress() const {
    return address;
}

bool Ip4Interface::isTargetOf(const Util::Network::Ip4::Ip4Address &targetAddress) {
    if (targetAddress == address || networkMask.createBroadcastAddress(networkAddress) == targetAddress || targetAddress.isBroadcastAddress()) {
        return true;
    }

    return false;
}

Device::Network::NetworkDevice &Ip4Interface::getDevice() const {
    return device;
}

}