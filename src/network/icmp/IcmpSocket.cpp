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

#include "kernel/system/System.h"
#include "kernel/service/NetworkService.h"
#include "lib/util/network/icmp/IcmpDatagram.h"
#include "IcmpSocket.h"
#include "lib/util/Exception.h"
#include "lib/util/network/NetworkAddress.h"
#include "network/NetworkStack.h"
#include "network/icmp/IcmpModule.h"

namespace Util {
namespace Network {
class Datagram;

namespace Ip4 {
class Ip4Address;
}  // namespace Ip4
}  // namespace Network
}  // namespace Util

namespace Network::Icmp {

IcmpSocket::~IcmpSocket() {
    auto &icmpModule = Kernel::System::getService<Kernel::NetworkService>().getNetworkStack().getIcmpModule();
    icmpModule.deregisterSocket(*this);
}

bool IcmpSocket::send(const Util::Network::Datagram &datagram) {
    const auto &icmpDatagram = reinterpret_cast<const Util::Network::Icmp::IcmpDatagram&>(datagram);
    IcmpModule::writePacket(icmpDatagram.getType(), icmpDatagram.getCode(), reinterpret_cast<const Util::Network::Ip4::Ip4Address&>(icmpDatagram.getRemoteAddress()), icmpDatagram.getData(), icmpDatagram.getDataLength());
    return true;
}

void IcmpSocket::performBind() {
    if (bindAddress->getType() != Util::Network::NetworkAddress::IP4) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Ip4Socket: Invalid bind address!");
    }

    auto &icmpModule = Kernel::System::getService<Kernel::NetworkService>().getNetworkStack().getIcmpModule();
    if (!icmpModule.registerSocket(*this)) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Failed to register ICMP socket!");
    }
}

}