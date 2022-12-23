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
 * The UDP/IP stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#include "ArpEntry.h"

#include "lib/util/network/NetworkAddress.h"

namespace Kernel::Network::Arp {

ArpEntry::ArpEntry(const Util::Network::Ip4::Ip4Address &protocolAddress, const Util::Network::MacAddress &hardwareAddress) :
        protocolAddress(protocolAddress), hardwareAddress(hardwareAddress) {}

const Util::Network::MacAddress& ArpEntry::getHardwareAddress() const {
    return hardwareAddress;
}

const Util::Network::Ip4::Ip4Address& ArpEntry::getProtocolAddress() const {
    return protocolAddress;
}

bool ArpEntry::operator!=(const ArpEntry &other) const {
    return hardwareAddress != other.hardwareAddress || protocolAddress != other.protocolAddress;
}

bool ArpEntry::operator==(const ArpEntry &other) const {
    return hardwareAddress == other.hardwareAddress && protocolAddress == other.protocolAddress;
}

void ArpEntry::setHardwareAddress(const Util::Network::MacAddress &hardwareAddress) {
    ArpEntry::hardwareAddress = hardwareAddress;
}

void ArpEntry::setProtocolAddress(const Util::Network::Ip4::Ip4Address &protocolAddress) {
    ArpEntry::protocolAddress = protocolAddress;
}

}