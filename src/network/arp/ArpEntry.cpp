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
 */

#include "ArpEntry.h"

namespace Network::Arp {

ArpEntry::ArpEntry(const Ip4::Ip4Address &protocolAddress, const MacAddress &hardwareAddress) :
        protocolAddress(protocolAddress), hardwareAddress(hardwareAddress) {}

const MacAddress& ArpEntry::getHardwareAddress() const {
    return hardwareAddress;
}

const Ip4::Ip4Address& ArpEntry::getProtocolAddress() const {
    return protocolAddress;
}

bool ArpEntry::operator!=(const ArpEntry &other) const {
    return hardwareAddress != other.hardwareAddress || protocolAddress != other.protocolAddress;
}

bool ArpEntry::operator==(const ArpEntry &other) const {
    return hardwareAddress == other.hardwareAddress && protocolAddress == other.protocolAddress;
}

void ArpEntry::setHardwareAddress(const MacAddress &hardwareAddress) {
    ArpEntry::hardwareAddress = hardwareAddress;
}

void ArpEntry::setProtocolAddress(const Ip4::Ip4Address &protocolAddress) {
    ArpEntry::protocolAddress = protocolAddress;
}

}