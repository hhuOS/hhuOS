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

#include "NetworkAddress.h"

#include "util/base/Address.h"
#include "util/io/stream/InputStream.h"
#include "util/io/stream/OutputStream.h"

namespace Util::Network {

NetworkAddress::NetworkAddress(const uint8_t length, const Type type) :
        buffer(new uint8_t[length]), length(length), type(type) {
    Address(buffer).setRange(0, length);
}

NetworkAddress::NetworkAddress(const uint8_t *buffer, const uint8_t length, const Type type) :
        NetworkAddress(length, type) {
    setAddress(buffer);
}

NetworkAddress::NetworkAddress(const NetworkAddress &other) : NetworkAddress(other.length, other.type) {
    setAddress(other.buffer);
}

NetworkAddress& NetworkAddress::operator=(const NetworkAddress &other) {
    if (&other == this) {
        return *this;
    }

    length = other.length;
    type = other.type;
    setAddress(other.buffer);

    return *this;
}

NetworkAddress::~NetworkAddress() {
    delete[] buffer;
}

bool NetworkAddress::operator==(const NetworkAddress &other) const {
    if (length != other.length) {
        return false;
    }

    return Address(buffer).compareRange(Address(other.buffer), length) == 0;
}

bool NetworkAddress::operator!=(const NetworkAddress &other) const {
    if (length != other.length) {
        return true;
    }

    return Address(buffer).compareRange(Address(other.buffer), length) != 0;
}

void NetworkAddress::read(Io::InputStream &stream) const {
    stream.read(buffer, 0, length);
}

void NetworkAddress::write(Io::OutputStream &stream) const {
    stream.write(buffer, 0, length);
}

void NetworkAddress::setAddress(const uint8_t *buffer) const {
    const auto source = Address(buffer);
    const auto destination = Address(NetworkAddress::buffer);
    destination.copyRange(source, length);
}

void NetworkAddress::getAddress(uint8_t *buffer) const {
    const auto source = Address(NetworkAddress::buffer);
    const auto destination = Address(buffer);
    destination.copyRange(source, length);
}

uint8_t NetworkAddress::getLength() const {
    return length;
}

NetworkAddress::Type NetworkAddress::getType() const {
    return type;
}

uint8_t NetworkAddress::compareTo(const NetworkAddress &other) const {
    uint8_t i = 0;
    uint8_t j = 0;

    for (i = 0; i < getLength() || i < other.getLength(); i++) {
        for (j = 0; j < 8; j++) {
            const auto bit = (buffer[i] >> j) & 0x01;
            const auto otherBit = (other.buffer[i] >> j) & 0x01;

            if (bit != otherBit) {
                return i * 8 + j;
            }
        }
    }

    return i * 8 + j;
}

}