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

#include "Socket.h"

#include "interface.h"
#include "util/base/Panic.h"
#include "util/base/String.h"
#include "util/collection/Array.h"
#include "util/collection/ArrayList.h"
#include "util/network/NetworkAddress.h"
#include "util/network/ip4/Ip4Route.h"
#include "util/network/ip4/Ip4Address.h"
#include "util/network/ip4/Ip4SubnetAddress.h"

namespace Util {
namespace Network {

Socket::Socket(const int32_t fileDescriptor, const Type type) : fileDescriptor(fileDescriptor), type(type) {}

Socket::~Socket() {
    closeFile(fileDescriptor);
}

Socket Socket::createSocket(const Type type) {
    const auto fileDescriptor = ::createSocket(type);
    if (fileDescriptor == -1) {
        Util::Panic::fire(Panic::ILLEGAL_STATE, "Failed to open socket!");
    }

    return {fileDescriptor, type};
}

void Socket::setTimeout(const Time::Timestamp timeout) const {
    controlFile(fileDescriptor, SET_TIMEOUT,
        Array<size_t>({static_cast<size_t>(timeout.toMilliseconds())}));
}

bool Socket::bind(const NetworkAddress &address) const {
    return controlFile(fileDescriptor, BIND,
        Array<size_t>({reinterpret_cast<size_t>(&address)}));
}

bool Socket::getLocalAddress(NetworkAddress &address) const {
    return controlFile(fileDescriptor, GET_LOCAL_ADDRESS,
        Array<size_t>({reinterpret_cast<size_t>(&address)}));
}

bool Socket::send(const Datagram &datagram) const {
    return sendDatagram(fileDescriptor, datagram);
}

bool Socket::receive(Datagram &datagram) const {
    return receiveDatagram(fileDescriptor, datagram);
}

Array<Ip4::Ip4SubnetAddress> Socket::getIp4Addresses() const {
    // The system call fills the given array with the addresses up to the array size.
    // If there are more addresses, the system call aborts when the array is full and returns successfully.
    // Therefore, we start with an array of size 1 and double the size
    // until the system call does not fill the array completely.
    auto addresses = Array<Ip4::Ip4SubnetAddress>(1);
    if (!controlFile(fileDescriptor, GET_IP4_ADDRESSES,
        Util::Array<uint32_t>({reinterpret_cast<uint32_t>(&addresses)})))
    {
        return Array<Ip4::Ip4SubnetAddress>(0);
    }

    // If the last address `ANY` ("0.0.0.0"), the address is empty and has not been overwritten by the system call.
    // This means we are finished. Otherwise, the array was not large enough and we try again with a larger array.
    while (addresses[addresses.length()- 1].getIp4Address() != Ip4::Ip4Address::ANY) {
        addresses = Array<Ip4::Ip4SubnetAddress>(addresses.length() * 2);
        if (!controlFile(fileDescriptor, GET_IP4_ADDRESSES,
            Array<uint32_t>({reinterpret_cast<uint32_t>(&addresses)})))
        {
            return Array<Ip4::Ip4SubnetAddress>(0);
        }
    }

    ArrayList<Ip4::Ip4SubnetAddress> ret;
    for (uint32_t i = 0; i < addresses.length() && addresses[i].getIp4Address() != Ip4::Ip4Address::ANY; i++) {
        ret.add(addresses[i]);
    }

    return ret.toArray();
}

bool Socket::removeIp4Address(const Ip4::Ip4SubnetAddress &address) const {
    return controlFile(fileDescriptor, REMOVE_IP4_ADDRESS,
        Array<uint32_t>({reinterpret_cast<uint32_t>(&address)}));
}

bool Socket::addIp4Address(const Ip4::Ip4SubnetAddress &address) const {
    return controlFile(fileDescriptor, ADD_IP4_ADDRESS,
        Array<uint32_t>({reinterpret_cast<uint32_t>(&address)}));
}

Array<Ip4::Ip4Route> Socket::getRoutes() const {
    // The system call fills the given arrays with the route parts up to the array size.
    // If there are more routes, the system call aborts when an array is full and returns successfully.
    // Therefore, we start with an array of size 1 and double the size
    // until the system call does not fill the arrays completely.
    auto sourceAddresses = Array<Ip4::Ip4Address>(1);
    auto targetAddresses = Array<Ip4::Ip4SubnetAddress>(1);
    auto nextHops = Array<Ip4::Ip4Address>(1);
    auto devices = Array<char*>(1);

    if (!controlFile(fileDescriptor, GET_ROUTES,
            Array<uint32_t>({
                reinterpret_cast<uint32_t>(&sourceAddresses),
                reinterpret_cast<uint32_t>(&targetAddresses),
                reinterpret_cast<uint32_t>(&nextHops),
                reinterpret_cast<uint32_t>(&devices)})))
    {
        return Array<Ip4::Ip4Route>(0);
    }

    // If the last device name is empty, the array has not been filled completely.
    // This means we are finished. Otherwise, the arrays were not large enough and we try again with larger arrays.
    while (!String(devices[devices.length() - 1]).isEmpty()) {
        sourceAddresses = Array<Ip4::Ip4Address>(sourceAddresses.length() * 2);
        targetAddresses = Array<Ip4::Ip4SubnetAddress>(targetAddresses.length() * 2);
        nextHops = Array<Ip4::Ip4Address>(nextHops.length() * 2);
        devices = Array<char*>(devices.length() * 2);
        for (auto &string: devices) {
            string = nullptr;
        }

        if (!controlFile(fileDescriptor, GET_ROUTES,
            Array<uint32_t>({
                reinterpret_cast<uint32_t>(&sourceAddresses),
                reinterpret_cast<uint32_t>(&targetAddresses),
                reinterpret_cast<uint32_t>(&nextHops),
                reinterpret_cast<uint32_t>(&devices)})))
        {
            return Array<Ip4::Ip4Route>(0);
        }
    }

    ArrayList<Ip4::Ip4Route> ret(sourceAddresses.length());
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
    return controlFile(fileDescriptor, REMOVE_ROUTE,
        Array<uint32_t>({reinterpret_cast<uint32_t>(&route)}));
}

bool Socket::addRoute(const Ip4::Ip4Route &route) const {
    return controlFile(fileDescriptor, ADD_ROUTE,
        Array<uint32_t>({reinterpret_cast<uint32_t>(&route)}));
}

bool Socket::setAccessMode(const Io::File::AccessMode accessMode) const {
    return Io::File::setAccessMode(fileDescriptor, accessMode);
}

bool Socket::isReadyToRead() const {
    return Io::File::isReadyToRead(fileDescriptor);
}

}
}