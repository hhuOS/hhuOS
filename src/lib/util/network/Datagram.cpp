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

#include "Datagram.h"

#include "util/base/Panic.h"
#include "util/base/Address.h"
#include "util/io/stream/ByteArrayOutputStream.h"
#include "util/network/NetworkAddress.h"
#include "util/network/MacAddress.h"
#include "util/network/ip4/Ip4Address.h"
#include "util/network/ip4/Ip4PortAddress.h"

namespace Util::Network {

Datagram::Datagram(const NetworkAddress::Type type) {
    switch (type) {
        case NetworkAddress::MAC:
            remoteAddress = new MacAddress();
            break;
        case NetworkAddress::IP4:
            remoteAddress = new Ip4::Ip4Address();
            break;
        case NetworkAddress::IP4_PORT:
            remoteAddress = new Ip4::Ip4PortAddress();
            break;
        default:
            Util::Panic::fire(Panic::INVALID_ARGUMENT, "Socket: Illegal address type for bind!");
    }
}

Datagram::Datagram(const uint8_t *buffer, const uint16_t length, const NetworkAddress &remoteAddress) :
        remoteAddress(remoteAddress.createCopy()), buffer(new uint8_t[length]), length(length) {
    Address(Datagram::buffer).copyRange(Address(buffer), length);
}

Datagram::Datagram(const Io::ByteArrayOutputStream &stream, const NetworkAddress &remoteAddress) :
        remoteAddress(remoteAddress.createCopy()), buffer(new uint8_t[stream.getPosition()]), length(stream.getPosition()) {
    Address(buffer).copyRange(Address(stream.getBuffer()), stream.getPosition());
}

Datagram::~Datagram() {
    delete remoteAddress;
    delete[] buffer;
}

const NetworkAddress& Datagram::getRemoteAddress() const {
    return *remoteAddress;
}

void Datagram::setRemoteAddress(const NetworkAddress &address) const {
    Io::ByteArrayOutputStream addressStream;
    address.write(addressStream);
    remoteAddress->setAddress(addressStream.getBuffer());
}

const uint8_t *Datagram::getData() const {
    return buffer;
}

uint32_t Datagram::getLength() const {
    return length;
}

void Datagram::setData(uint8_t *buffer, const uint32_t length) {
    delete[] Datagram::buffer;
    Datagram::buffer = buffer;
    Datagram::length = length;
}

}