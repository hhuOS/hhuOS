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

#include "Socket.h"

#include "lib/interface.h"
#include "lib/util/base/Exception.h"
#include "lib/util/network/ip4/Ip4Route.h"
#include "lib/util/collection/Array.h"
#include "lib/util/network/NetworkAddress.h"
#include "lib/util/network/ip4/Ip4Address.h"
#include "lib/util/network/ip4/Ip4SubnetAddress.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/base/String.h"

namespace Util {
namespace Network {
class Datagram;
}  // namespace Network
}  // namespace Util

namespace Util::Network {

Socket::Socket(int32_t fileDescriptor, Type type) : fileDescriptor(fileDescriptor), type(type) {}

Socket::~Socket() {
    ::closeFile(fileDescriptor);
}

Socket Socket::createSocket(Socket::Type type) {
    auto fileDescriptor = ::createSocket(type);
    if (fileDescriptor == -1) {
        Util::Exception::throwException(Exception::ILLEGAL_STATE, "Failed to open socket!");
    }

    return Socket(fileDescriptor, type);
}

void Socket::setTimeout(uint32_t timeout) const {
    ::controlFile(fileDescriptor, SET_TIMEOUT, Util::Array<uint32_t>({timeout}));
}

bool Socket::bind(const NetworkAddress &address) const {
    return ::controlFile(fileDescriptor, BIND, Util::Array<uint32_t>({reinterpret_cast<uint32_t>(&address)}));
}

bool Socket::getLocalAddress(NetworkAddress &address) const {
    return ::controlFile(fileDescriptor, GET_LOCAL_ADDRESS, Util::Array<uint32_t>({reinterpret_cast<uint32_t>(&address)}));
}

bool Socket::send(const Datagram &datagram) const {
    return ::sendDatagram(fileDescriptor, datagram);
}

bool Socket::receive(Util::Network::Datagram &datagram) const {
    return ::receiveDatagram(fileDescriptor, datagram);
}

Array<Ip4::Ip4SubnetAddress> Socket::getIp4Addresses() const {
    uint32_t size = 1;
    auto addresses = Array<Ip4::Ip4SubnetAddress>(size);
    if (!::controlFile(fileDescriptor, GET_IP4_ADDRESSES, Util::Array<uint32_t>({reinterpret_cast<uint32_t>(&addresses)}))) {
        return Array<Ip4::Ip4SubnetAddress>(0);
    }

    while (addresses[size - 1].getIp4Address() != Ip4::Ip4Address::ANY) {
        size *= 2;
        addresses = Array<Ip4::Ip4SubnetAddress>(size);
        if (!::controlFile(fileDescriptor, GET_IP4_ADDRESSES, Util::Array<uint32_t>({reinterpret_cast<uint32_t>(&addresses)}))) {
            return Array<Ip4::Ip4SubnetAddress>(0);
        }
    }

    auto ret = ArrayList<Ip4::Ip4SubnetAddress>();
    for (uint32_t i = 0; i < addresses.length() && addresses[i].getIp4Address() != Ip4::Ip4Address::ANY; i++) {
        ret.add(addresses[i]);
    }

    return ret.toArray();
}

bool Socket::removeIp4Address(const Ip4::Ip4SubnetAddress &address) const {
    return ::controlFile(fileDescriptor, REMOVE_IP4_ADDRESS, Util::Array<uint32_t>({reinterpret_cast<uint32_t>(&address)}));
}

bool Socket::addIp4Address(const Ip4::Ip4SubnetAddress &address) const {
    return ::controlFile(fileDescriptor, ADD_IP4_ADDRESS, Util::Array<uint32_t>({reinterpret_cast<uint32_t>(&address)}));
}

Array<Ip4::Ip4Route> Socket::getRoutes() const {
    auto sourceAddresses = Array<Ip4::Ip4Address>(0);
    auto targetAddresses = Array<Ip4::Ip4SubnetAddress>(0);
    auto nextHops = Array<Ip4::Ip4Address>(0);
    auto devices = Array<char*>(0);
    uint32_t size = 1;

    do {
        size *= 2;
        sourceAddresses = Array<Ip4::Ip4Address>(size);
        targetAddresses = Array<Ip4::Ip4SubnetAddress>(size);
        nextHops = Array<Ip4::Ip4Address>(size);
        devices = Array<char*>(size);
        for (auto &string: devices) {
            string = nullptr;
        }

        if (!::controlFile(fileDescriptor, GET_ROUTES, Util::Array<uint32_t>({reinterpret_cast<uint32_t>(&sourceAddresses), reinterpret_cast<uint32_t>(&targetAddresses), reinterpret_cast<uint32_t>(&nextHops), reinterpret_cast<uint32_t>(&devices)}))) {
            return Array<Ip4::Ip4Route>(0);
        }
    } while (!String(devices[size - 1]).isEmpty());

    auto ret = Util::ArrayList<Ip4::Ip4Route>(sourceAddresses.length());
    for (uint32_t i = 0; i < sourceAddresses.length() && !String(devices[i]).isEmpty(); i++) {
        if (nextHops[i] == Ip4::Ip4Address::ANY) {
            ret.add(Ip4::Ip4Route(sourceAddresses[i], targetAddresses[i], devices[i]));
        } else {
            ret.add(Ip4::Ip4Route(sourceAddresses[i], targetAddresses[i], nextHops[i], devices[i]));
        }

        delete devices[i];
    }

    return ret.toArray();
}

bool Socket::removeRoute(const Ip4::Ip4Route &route) const {
    return ::controlFile(fileDescriptor, REMOVE_ROUTE, Util::Array<uint32_t>({reinterpret_cast<uint32_t>(&route)}));
}

bool Socket::addRoute(const Ip4::Ip4Route &route) const {
    return ::controlFile(fileDescriptor, ADD_ROUTE, Util::Array<uint32_t>({reinterpret_cast<uint32_t>(&route)}));
}

bool Socket::setAccessMode(Util::Io::File::AccessMode accessMode) const {
    return Util::Io::File::setAccessMode(fileDescriptor, accessMode);
}

bool Socket::isReadyToRead() const {
    return Util::Io::File::isReadyToRead(fileDescriptor);
}

}