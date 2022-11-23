/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Hannes Feil,  Michael Schoettner
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

#include "Ip4Address.h"
#include "lib/util/memory/Address.h"
#include "lib/util/memory/String.h"

namespace Network::Ip4 {

Ip4Address::Ip4Address(uint8_t *buffer) {
    setAddress(buffer);
}

Ip4Address::Ip4Address(const Util::Memory::String &string) {
    auto split = Util::Memory::String(string).split(".");
    uint8_t buffer[4] = {
            static_cast<uint8_t>(Util::Memory::String::parseInt(split[0])),
            static_cast<uint8_t>(Util::Memory::String::parseInt(split[1])),
            static_cast<uint8_t>(Util::Memory::String::parseInt(split[2])),
            static_cast<uint8_t>(Util::Memory::String::parseInt(split[3])),
    };
    setAddress(buffer);
}

Ip4Address::Ip4Address(const Ip4Address &other) {
    setAddress(other.buffer);
}

Ip4Address &Ip4Address::operator=(const Ip4Address &other) {
    if (&other == this) {
        return *this;
    }

    setAddress(other.buffer);
    return *this;
}

void Ip4Address::read(Util::Stream::InputStream &stream) {
    stream.read(buffer, 0, ADDRESS_LENGTH);
}

void Ip4Address::write(Util::Stream::OutputStream &stream) const {
    stream.write(buffer, 0, ADDRESS_LENGTH);
}

void Ip4Address::setAddress(const Util::Memory::String &string) {
    auto split = string.split(".");
    if (split.length() != 4) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Ip4Address: Invalid address string given!");
    }

    for (uint8_t i = 0; i < ADDRESS_LENGTH; i++) {
        buffer[i] = Util::Memory::String::parseInt(split[i]);
    }
}

void Ip4Address::setAddress(const uint8_t *buffer) {
    auto source = Util::Memory::Address<uint32_t>(buffer);
    auto destination = Util::Memory::Address<uint32_t>(buffer);
    destination.copyRange(source, ADDRESS_LENGTH);
}

void Ip4Address::getAddress(uint8_t *buffer) const {
    for (uint8_t i = 0; i < ADDRESS_LENGTH; i++) {
        buffer[i] = buffer[i];
    }
}

uint8_t Ip4Address::getLength() const {
    return ADDRESS_LENGTH;
}

Util::Memory::String Ip4Address::toString() const {
    return Util::Memory::String::format("%03u.%03u.%03u.%03u", buffer[0], buffer[1], buffer[2], buffer[3]);
}

NetworkAddress *Ip4Address::createCopy() const {
    return new Ip4Address(*this);
}

NetworkAddress::Type Ip4Address::getType() const {
    return NetworkAddress::IP4;
}

}