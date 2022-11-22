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

MacAddress MacAddress::createBroadcastAddress() {
    uint8_t buffer[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    return MacAddress(buffer);
}

MacAddress::Address MacAddress::getAddress() const {
    return address;
}

void MacAddress::setAddress(uint8_t *buffer) {
    auto source = Util::Memory::Address<uint32_t>(buffer);
    auto destination = Util::Memory::Address<uint32_t>(address.buffer);
    destination.copyRange(source, ADDRESS_LENGTH);
}

void MacAddress::read(Util::Stream::InputStream &stream) {
    stream.read(address.buffer, 0, ADDRESS_LENGTH);
}

void MacAddress::write(Util::Stream::OutputStream &stream) const {
    stream.write(address.buffer, 0, ADDRESS_LENGTH);
}

bool MacAddress::isBroadcastAddress() const {
    for (const auto c : address.buffer) {
        if (c != 0xff) return false;
    }

    return true;
}

bool MacAddress::operator!=(const MacAddress &other) const {
    auto first = Util::Memory::Address<uint32_t>(address.buffer);
    auto second = Util::Memory::Address<uint32_t>(other.address.buffer);
    return first.compareRange(second, ADDRESS_LENGTH) != 0;
}

bool MacAddress::operator==(const MacAddress &other) const {
    auto first = Util::Memory::Address<uint32_t>(address.buffer);
    auto second = Util::Memory::Address<uint32_t>(other.address.buffer);
    return first.compareRange(second, ADDRESS_LENGTH) == 0;
}

}