/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#include "Socket.h"

#include "NetworkAddress.h"
#include "ip4/Ip4Route.h"
#include "ip4/Ip4Address.h"
#include "ip4/Ip4SubnetAddress.h"

#include <interface.h>
#include <util/base/Panic.h>
#include <util/base/String.h>
#include <util/collection/Array.h>
#include <util/collection/ArrayList.h>

namespace Util {
namespace Network {

Socket::Socket(const int32_t fileDescriptor, const Type type) : fileDescriptor(fileDescriptor), type(type) {}

Socket::Socket(const Type type) : type(type) {
    fileDescriptor = createSocket(type);
    if (fileDescriptor == -1) {
        Util::Panic::fire(Panic::ILLEGAL_STATE, "Failed to open socket!");
    }
}

Socket::~Socket() {
    closeFile(fileDescriptor);
}

void Socket::setTimeout(const Time::Timestamp timeout) const {
    Io::File::controlFile(fileDescriptor, SET_TIMEOUT, timeout.toMilliseconds());
}

bool Socket::bind(const NetworkAddress &address) const {
    return Io::File::controlFile(fileDescriptor, BIND, reinterpret_cast<size_t>(address.getBuffer())) == 0;
}

bool Socket::getLocalAddress(NetworkAddress &address) const {
    return Io::File::controlFile(fileDescriptor, GET_LOCAL_ADDRESS,
        reinterpret_cast<size_t>(address.getBuffer())) == 0;
}

bool Socket::send(const Datagram &datagram) const {
    return sendDatagram(fileDescriptor, type, datagram);
}

const Datagram* Socket::receive() const {
    return receiveDatagram(fileDescriptor, type);
}

Array<Ip4::Ip4SubnetAddress> Socket::getIp4Addresses() const {
    uint8_t *buffer;
    const auto count = Io::File::controlFile(fileDescriptor, GET_IP4_ADDRESSES,
        reinterpret_cast<size_t>(&buffer));

    if (count <= 0) {
        return Array<Ip4::Ip4SubnetAddress>(0);
    }

    Array<Ip4::Ip4SubnetAddress> addresses(count);
    for (size_t i = 0; i < count; i++) {
        addresses[i] = Ip4::Ip4SubnetAddress(buffer + i * Ip4::Ip4SubnetAddress::ADDRESS_LENGTH);
    }

    delete buffer;
    return addresses;
}

bool Socket::removeIp4Address(const Ip4::Ip4SubnetAddress &address) const {
    return Io::File::controlFile(fileDescriptor, REMOVE_IP4_ADDRESS,
        reinterpret_cast<size_t>(address.getBuffer())) == 0;
}

bool Socket::addIp4Address(const Ip4::Ip4SubnetAddress &address) const {
    return Io::File::controlFile(fileDescriptor, ADD_IP4_ADDRESS,
        reinterpret_cast<size_t>(address.getBuffer())) == 0;
}

Array<Ip4::Ip4Route> Socket::getRoutes() const {
    uint8_t *addressBuffer;
    char **deviceBuffer;
    const auto count = Io::File::controlFile(fileDescriptor, GET_ROUTES,
        reinterpret_cast<size_t>(&addressBuffer), reinterpret_cast<size_t>(&deviceBuffer));

    auto routes = Array<Ip4::Ip4Route>(count);
    for (size_t i = 0; i < count; i++) {
        constexpr auto ENTRY_LENGTH = 2 * Ip4::Ip4SubnetAddress::ADDRESS_LENGTH +
            Ip4::Ip4SubnetAddress::ADDRESS_LENGTH;

        auto sourceAddress = Ip4::Ip4Address(addressBuffer + i * ENTRY_LENGTH);
        auto targetAddress = Ip4::Ip4SubnetAddress(addressBuffer + i * ENTRY_LENGTH +
            Ip4::Ip4Address::ADDRESS_LENGTH);
        auto nextHop = Ip4::Ip4Address(addressBuffer + i * ENTRY_LENGTH + Ip4::Ip4Address::ADDRESS_LENGTH +
            Ip4::Ip4SubnetAddress::ADDRESS_LENGTH);

        if (nextHop == Ip4::Ip4Address::ANY) {
            routes[i] = Ip4::Ip4Route(sourceAddress, targetAddress, String(deviceBuffer[i]));
        } else {
            routes[i] = Ip4::Ip4Route(sourceAddress, targetAddress, nextHop, String(deviceBuffer[i]));
        }
    }

    for (size_t i = 0; i < count; i++) {
        delete deviceBuffer[i];
    }
    delete addressBuffer;
    delete deviceBuffer;

    return routes;
}

bool Socket::removeRoute(const Ip4::Ip4Route &route) const {
    uint8_t buffer[2 * Ip4::Ip4SubnetAddress::ADDRESS_LENGTH + Ip4::Ip4SubnetAddress::ADDRESS_LENGTH];
    route.getSourceAddress().getAddress(buffer);
    route.getTargetAddress().getAddress(buffer + Ip4::Ip4Address::ADDRESS_LENGTH);

    if (route.hasNextHop()) {
        route.getNextHop().getAddress(buffer + Ip4::Ip4Address::ADDRESS_LENGTH +
            Ip4::Ip4SubnetAddress::ADDRESS_LENGTH);
    } else {
        Ip4::Ip4Address::ANY.getAddress(buffer + Ip4::Ip4Address::ADDRESS_LENGTH +
            Ip4::Ip4SubnetAddress::ADDRESS_LENGTH);
    }

    return Io::File::controlFile(fileDescriptor, REMOVE_ROUTE, reinterpret_cast<size_t>(buffer),
        reinterpret_cast<size_t>(static_cast<const char*>(route.getDeviceIdentifier()))) == 0;
}

bool Socket::addRoute(const Ip4::Ip4Route &route) const {
    uint8_t buffer[2 * Ip4::Ip4SubnetAddress::ADDRESS_LENGTH + Ip4::Ip4SubnetAddress::ADDRESS_LENGTH];
    route.getSourceAddress().getAddress(buffer);
    route.getTargetAddress().getAddress(buffer + Ip4::Ip4Address::ADDRESS_LENGTH);

    if (route.hasNextHop()) {
        route.getNextHop().getAddress(buffer + Ip4::Ip4Address::ADDRESS_LENGTH +
            Ip4::Ip4SubnetAddress::ADDRESS_LENGTH);
    } else {
        Ip4::Ip4Address::ANY.getAddress(buffer + Ip4::Ip4Address::ADDRESS_LENGTH +
            Ip4::Ip4SubnetAddress::ADDRESS_LENGTH);
    }

    return Io::File::controlFile(fileDescriptor, ADD_ROUTE, reinterpret_cast<size_t>(buffer),
        reinterpret_cast<size_t>(static_cast<const char*>(route.getDeviceIdentifier()))) == 0;
}

bool Socket::setAccessMode(const Io::File::AccessMode accessMode) const {
    return Io::File::setAccessMode(fileDescriptor, accessMode);
}

bool Socket::isReadyToRead() const {
    return Io::File::isReadyToRead(fileDescriptor);
}

}
}