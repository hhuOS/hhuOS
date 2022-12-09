/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Hannes Feil, Michael Schoettner
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

#include "kernel/system/System.h"
#include "kernel/service/NetworkService.h"
#include "device/network/NetworkDevice.h"
#include "Ip4Route.h"

namespace Network::Ip4 {

Ip4Route::Ip4Route(const Ip4Address &localAddress, const Ip4NetworkMask &networkMask, const Ip4Address &nextHop, const Util::Memory::String &device) :
        address(localAddress), networkMask(networkMask), nextHop(nextHop), interface(&Kernel::System::getService<Kernel::NetworkService>().getNetworkStack().getIp4Module().getInterface(device)), nextHopValid(false) {}

Ip4Route::Ip4Route(const Ip4Address &localAddress, const Ip4NetworkMask &networkMask, const Util::Memory::String &device) :
        address(localAddress), networkMask(networkMask), nextHop(), interface(&Kernel::System::getService<Kernel::NetworkService>().getNetworkStack().getIp4Module().getInterface(device)), nextHopValid(false) {}

bool Ip4Route::operator==(const Ip4Route &other) const {
    return address == other.address && networkMask == other.networkMask;
}

bool Ip4Route::operator!=(const Ip4Route &other) const {
    return address != other.address || networkMask != other.networkMask;
}

const Ip4Address& Ip4Route::getAddress() const {
    return address;
}

const Ip4NetworkMask& Ip4Route::getNetworkMask() const {
    return networkMask;
}

const Ip4Interface& Ip4Route::getInterface() const {
    return *interface;
}

bool Ip4Route::hasNextHop() const {
    return nextHopValid;
}

const Ip4Address &Ip4Route::getNextHop() const {
    return nextHop;
}

}