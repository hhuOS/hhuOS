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
 * The network stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#include "Socket.h"

#include "lib/interface.h"
#include "lib/util/base/Exception.h"
#include "lib/util/collection/Array.h"

namespace Util {
namespace Network {
class Datagram;
namespace Ip4 {
class Ip4Address;
class Ip4NetworkMask;
}  // namespace Ip4
}  // namespace Network
}  // namespace Util

namespace Util::Network {

Socket::Socket(int32_t fileDescriptor) : fileDescriptor(fileDescriptor) {}

Socket::~Socket() {
    ::closeFile(fileDescriptor);
}

Socket Socket::createSocket(Socket::Type socketType) {
    auto fileDescriptor = ::createSocket(socketType);
    if (fileDescriptor == -1) {
        Util::Exception::throwException(Exception::ILLEGAL_STATE, "Failed to open socket!");
    }

    return Socket(fileDescriptor);
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

bool Socket::getIp4Address(Ip4::Ip4Address &address, Ip4::Ip4NetworkMask &mask) const {
    return ::controlFile(fileDescriptor, GET_IP4_ADDRESS, Util::Array<uint32_t>({reinterpret_cast<uint32_t>(&address), reinterpret_cast<uint32_t>(&mask)}));
}

bool Socket::removeIp4Address(const Ip4::Ip4Address &address, const Ip4::Ip4NetworkMask &mask) const {
    return ::controlFile(fileDescriptor, REMOVE_IP4_ADDRESS, Util::Array<uint32_t>({reinterpret_cast<uint32_t>(&address), reinterpret_cast<uint32_t>(&mask)}));
}

bool Socket::setIp4Address(const Ip4::Ip4Address &address, const Ip4::Ip4NetworkMask &mask) const {
    return ::controlFile(fileDescriptor, SET_IP4_ADDRESS, Util::Array<uint32_t>({reinterpret_cast<uint32_t>(&address), reinterpret_cast<uint32_t>(&mask)}));
}

}