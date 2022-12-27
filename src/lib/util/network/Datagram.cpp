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
 * The network stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#include "lib/util/network/Datagram.h"

#include "lib/util/memory/Address.h"
#include "lib/util/network/NetworkAddress.h"
#include "MacAddress.h"
#include "lib/util/network/ip4/Ip4Address.h"
#include "lib/util/network/ip4/Ip4PortAddress.h"
#include "lib/util/stream/ByteArrayOutputStream.h"
#include "lib/util/Exception.h"

namespace Util::Network {

Datagram::Datagram(NetworkAddress::Type type) {
    switch (type) {
        case Util::Network::NetworkAddress::MAC:
            remoteAddress = new Util::Network::MacAddress();
            break;
        case Util::Network::NetworkAddress::IP4:
            remoteAddress = new Util::Network::Ip4::Ip4Address();
            break;
        case Util::Network::NetworkAddress::IP4_PORT:
            remoteAddress = new Util::Network::Ip4::Ip4PortAddress();
            break;
        default:
            Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Socket: Illegal address type for bind!");
    }
}

Datagram::Datagram(const uint8_t *buffer, uint16_t length, const Util::Network::NetworkAddress &remoteAddress) :
        remoteAddress(remoteAddress.createCopy()), buffer(new uint8_t[length]), length(length) {
    Util::Memory::Address<uint32_t>(Datagram::buffer).copyRange(Util::Memory::Address<uint32_t>(buffer), length);
}

Datagram::Datagram(uint8_t *buffer, uint16_t length, const NetworkAddress &remoteAddress) :
        remoteAddress(remoteAddress.createCopy()), buffer(buffer), length(length) {}

Datagram::Datagram(const Stream::ByteArrayOutputStream &stream, const NetworkAddress &remoteAddress) :
        remoteAddress(remoteAddress.createCopy()), buffer(new uint8_t[stream.getLength()]), length(stream.getLength()) {
    Util::Memory::Address<uint32_t>(Datagram::buffer).copyRange(Util::Memory::Address<uint32_t>(stream.getBuffer()), stream.getLength());
}

Datagram::~Datagram() {
    delete remoteAddress;
    delete[] buffer;
}

const NetworkAddress &Network::Datagram::getRemoteAddress() const {
    return *remoteAddress;
}

void Datagram::setRemoteAddress(const NetworkAddress &address) {
    auto addressStream = Util::Stream::ByteArrayOutputStream();
    address.write(addressStream);
    remoteAddress->setAddress(addressStream.getBuffer());
}

uint8_t *Datagram::getData() const {
    return buffer;
}

uint32_t Datagram::getLength() const {
    return length;
}

void Datagram::setData(uint8_t *buffer, uint32_t length) {
    delete[] Datagram::buffer;
    Datagram::buffer = buffer;
    Datagram::length = length;
}

}