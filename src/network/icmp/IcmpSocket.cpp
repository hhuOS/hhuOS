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

#include "kernel/system/System.h"
#include "kernel/service/NetworkService.h"
#include "IcmpDatagram.h"
#include "IcmpSocket.h"
#include "lib/util/Exception.h"
#include "lib/util/network/NetworkAddress.h"
#include "network/NetworkStack.h"
#include "network/icmp/IcmpModule.h"

namespace Util {
namespace Network {
namespace Ip4 {
class Ip4Address;
}  // namespace Ip4
}  // namespace Network
}  // namespace Util

namespace Network {
class Datagram;
}  // namespace Network

namespace Network::Icmp {

IcmpSocket::~IcmpSocket() {
    auto &icmpModule = Kernel::System::getService<Kernel::NetworkService>().getNetworkStack().getIcmpModule();
    icmpModule.deregisterSocket(*this);
}

void IcmpSocket::send(const Datagram &datagram) {
    const auto &icmpDatagram = reinterpret_cast<const IcmpDatagram&>(datagram);
    IcmpModule::writePacket(icmpDatagram.getType(), icmpDatagram.getCode(), reinterpret_cast<const Util::Network::Ip4::Ip4Address&>(icmpDatagram.getRemoteAddress()), icmpDatagram.getData(), icmpDatagram.getDataLength());
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