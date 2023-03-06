/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "Ip4Route.h"

#include "lib/util/base/Exception.h"
#include "lib/util/network/NetworkAddress.h"
#include "lib/util/network/ip4/Ip4SubnetAddress.h"

namespace Util::Network::Ip4 {

Ip4Route::Ip4Route(const Util::Network::Ip4::Ip4SubnetAddress &localAddress, const Util::Network::Ip4::Ip4Address &nextHop, const Util::String &deviceIdentifier) :
        address(localAddress), nextHop(nextHop), deviceIdentifier(deviceIdentifier), nextHopValid(true) {}

Ip4Route::Ip4Route(const Util::Network::Ip4::Ip4SubnetAddress &localAddress, const Util::String &deviceIdentifier) :
        address(localAddress), nextHop(), deviceIdentifier(deviceIdentifier), nextHopValid(false) {}

bool Ip4Route::operator==(const Ip4Route &other) const {
    return address == other.address && deviceIdentifier == other.deviceIdentifier && nextHop == other.nextHop;
}

bool Ip4Route::operator!=(const Ip4Route &other) const {
    return address != other.address || deviceIdentifier != other.deviceIdentifier || nextHop != other.nextHop;
}

const Ip4SubnetAddress& Ip4Route::getAddress() const {
    return address;
}

Ip4Address Ip4Route::getSourceAddress() const {
    return address.getIp4Address();
}

Ip4SubnetAddress Ip4Route::getTargetAddress() const {
    return address.getSubnetAddress();
}

const Ip4::Ip4Address &Ip4Route::getNextHop() const {
    if (!nextHopValid) {
        Util::Exception::throwException(Exception::UNSUPPORTED_OPERATION, "Ip4Route: Route has no next hop!");
    }

    return nextHop;
}

const String& Ip4Route::getDeviceIdentifier() const {
    return deviceIdentifier;
}

bool Ip4Route::hasNextHop() const {
    return nextHopValid;
}

bool Ip4Route::isValid() {
    return !deviceIdentifier.isEmpty();
}

}