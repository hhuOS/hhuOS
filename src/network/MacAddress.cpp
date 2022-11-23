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

#include "MacAddress.h"
#include "lib/util/memory/Address.h"

namespace Network {

MacAddress::MacAddress(uint8_t *buffer) {
    setAddress(buffer);
}

MacAddress::MacAddress(const MacAddress &other) {
    setAddress(other.buffer);
}

MacAddress &MacAddress::operator=(const MacAddress &other) {
    if (&other == this) {
        return *this;
    }

    setAddress(other.buffer);
    return *this;
}

MacAddress MacAddress::createBroadcastAddress() {
    uint8_t buffer[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    return MacAddress(buffer);
}

void MacAddress::setAddress(const uint8_t *buffer) {
    auto source = Util::Memory::Address<uint32_t>(buffer);
    auto destination = Util::Memory::Address<uint32_t>(MacAddress::buffer);
    destination.copyRange(source, ADDRESS_LENGTH);
}

void MacAddress::read(Util::Stream::InputStream &stream) {
    stream.read(buffer, 0, ADDRESS_LENGTH);
}

void MacAddress::write(Util::Stream::OutputStream &stream) const {
    stream.write(buffer, 0, ADDRESS_LENGTH);
}

bool MacAddress::isBroadcastAddress() const {
    for (uint8_t i = 0; i < ADDRESS_LENGTH; i++) {
        if (buffer[i] != 0xff) return false;
    }

    return true;
}

Util::Memory::String MacAddress::toString() const {
    return Util::Memory::String::format("%02x:%02x:%02x:%02x:%02x:%02x\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
}

void MacAddress::setAddress(const Util::Memory::String &string) {
    Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "MacAddress: Cannot be parsed from string!");
}

void MacAddress::getAddress(uint8_t *buffer) const {
    for (uint8_t i = 0; i < ADDRESS_LENGTH; i++) {
        buffer[i] = MacAddress::buffer[i];
    }
}

uint8_t MacAddress::getLength() const {
    return ADDRESS_LENGTH;
}

NetworkAddress *MacAddress::createCopy() const {
    return new MacAddress(*this);
}

NetworkAddress::Type MacAddress::getType() const {
    return NetworkAddress::MAC;
}

}