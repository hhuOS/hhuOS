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

#include "lib/util/memory/Address.h"
#include "NetworkAddress.h"
#include "lib/util/stream/InputStream.h"
#include "lib/util/stream/OutputStream.h"

namespace Util::Network {

NetworkAddress::NetworkAddress(uint8_t length, NetworkAddress::Type type) : buffer(new uint8_t[length]), length(length), type(type) {
    Util::Memory::Address<uint32_t>(buffer).setRange(0, length);
}

NetworkAddress::NetworkAddress(const uint8_t *buffer, uint8_t length, NetworkAddress::Type type) : NetworkAddress(length, type) {
    setAddress(buffer);
}

NetworkAddress::NetworkAddress(const NetworkAddress &other) : NetworkAddress(other.length, other.type) {
    setAddress(other.buffer);
}

NetworkAddress &NetworkAddress::operator=(const NetworkAddress &other) {
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
    if (getLength() != other.getLength()) {
        return false;
    }

    auto first = Util::Memory::Address<uint32_t>(buffer);
    auto second = Util::Memory::Address<uint32_t>(other.buffer);
    auto result = first.compareRange(second, getLength());

    return result == 0;
}

bool NetworkAddress::operator!=(const NetworkAddress &other) const {
    return !(*this == other);
}

void NetworkAddress::read(Util::Stream::InputStream &stream) {
    stream.read(buffer, 0, length);
}

void NetworkAddress::write(Util::Stream::OutputStream &stream) const {
    stream.write(buffer, 0, length);
}

void NetworkAddress::setAddress(const uint8_t *buffer) {
    auto source = Util::Memory::Address<uint32_t>(buffer);
    auto destination = Util::Memory::Address<uint32_t>(NetworkAddress::buffer);
    destination.copyRange(source, length);
}

void NetworkAddress::setAddress(const NetworkAddress &other) {
    auto source = Util::Memory::Address<uint32_t>(other.buffer);
    auto destination = Util::Memory::Address<uint32_t>(buffer);
    destination.copyRange(source, length < other.length ? length : other.length);
}

void NetworkAddress::getAddress(uint8_t *buffer) const {
    auto source = Util::Memory::Address<uint32_t>(NetworkAddress::buffer);
    auto destination = Util::Memory::Address<uint32_t>(buffer);
    destination.copyRange(source, length);
}

uint8_t NetworkAddress::getLength() const {
    return length;
}

NetworkAddress::Type NetworkAddress::getType() const {
    return type;
}

uint8_t NetworkAddress::compareTo(const NetworkAddress &other) const {
    uint8_t i, j;

    for (i = 0; i < getLength() || i < other.getLength(); i++) {
        for (j = 0; j < 8; j++) {
            auto first = (buffer[i] >> j) & 0x01;
            auto second = (other.buffer[i] >> j) & 0x01;
            if (first != second) {
                return i * 8 + j;
            }
        }
    }

    return i * 8 + j;
}

}