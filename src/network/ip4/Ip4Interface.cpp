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

#include "Ip4Interface.h"

namespace Network::Ip4 {

Ip4Interface::Ip4Interface(const Network::Ip4::Ip4Address &address, const Network::Ip4::Ip4Address &networkAddress,
                           const Network::Ip4::Ip4NetworkMask &networkMask, Device::Network::NetworkDevice &device) :
        address(address), networkAddress(networkAddress), networkMask(networkMask), device(device) {}

Util::Memory::String Ip4Interface::getDeviceIdentifier() const {
    return device.getIdentifier();
}

void Ip4Interface::sendPacket(uint8_t *packet, uint32_t length) {
    device.sendPacket(packet, length);
}

const Ip4Address& Ip4Interface::getAddress() const {
    return address;
}

bool Ip4Interface::isTargetOf(const Ip4Address &targetAddress) {
    if (targetAddress == address || networkMask.createBroadcastAddress(networkAddress) == targetAddress || targetAddress.isBroadcastAddress()) {
        return true;
    }

    return false;
}

Device::Network::NetworkDevice &Ip4Interface::getDevice() const {
    return device;
}

}