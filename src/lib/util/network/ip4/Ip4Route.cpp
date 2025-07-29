/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include "base/Panic.h"
#include "network/NetworkAddress.h"
#include "network/ip4/Ip4SubnetAddress.h"

namespace Util::Network::Ip4 {

Ip4Route::Ip4Route(const Ip4SubnetAddress &targetAddress, const String &deviceIdentifier) :
        sourceAddress(targetAddress.getIp4Address()), targetAddress(targetAddress),
        deviceIdentifier(deviceIdentifier) {}

Ip4Route::Ip4Route(const Ip4Address &sourceAddress, const Ip4SubnetAddress &targetAddress, const String &deviceIdentifier) :
        sourceAddress(sourceAddress), targetAddress(targetAddress), deviceIdentifier(deviceIdentifier) {}

Ip4Route::Ip4Route(const Ip4SubnetAddress &targetAddress, const Ip4Address &nextHop, const String &deviceIdentifier) :
        sourceAddress(targetAddress.getIp4Address()), targetAddress(targetAddress),
        nextHop(nextHop), deviceIdentifier(deviceIdentifier), nextHopValid(true) {}

Ip4Route::Ip4Route(const Ip4Address &sourceAddress, const Ip4SubnetAddress &targetAddress, const Ip4Address &nextHop,
        const String &deviceIdentifier) :
        sourceAddress(sourceAddress), targetAddress(targetAddress), nextHop(nextHop),
        deviceIdentifier(deviceIdentifier), nextHopValid(true) {}

bool Ip4Route::operator==(const Ip4Route &other) const {
    return targetAddress == other.targetAddress &&
        deviceIdentifier == other.deviceIdentifier &&
        nextHop == other.nextHop;
}

bool Ip4Route::operator!=(const Ip4Route &other) const {
    return targetAddress != other.targetAddress ||
        deviceIdentifier != other.deviceIdentifier ||
        nextHop != other.nextHop;
}

const Ip4Address& Ip4Route::getSourceAddress() const {
    return sourceAddress;
}

Ip4SubnetAddress Ip4Route::getTargetAddress() const {
    return targetAddress.getSubnetAddress();
}

const Ip4Address& Ip4Route::getNextHop() const {
    if (!nextHopValid) {
        Util::Panic::fire(Panic::UNSUPPORTED_OPERATION, "Ip4Route: Route has no next hop!");
    }

    return nextHop;
}

const String& Ip4Route::getDeviceIdentifier() const {
    return deviceIdentifier;
}

bool Ip4Route::hasNextHop() const {
    return nextHopValid;
}

bool Ip4Route::isValid() const {
    return !deviceIdentifier.isEmpty();
}

}