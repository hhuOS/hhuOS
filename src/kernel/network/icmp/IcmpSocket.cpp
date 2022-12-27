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

#include "kernel/system/System.h"
#include "kernel/service/NetworkService.h"
#include "lib/util/network/icmp/IcmpDatagram.h"
#include "IcmpSocket.h"
#include "kernel/network/NetworkStack.h"
#include "kernel/network/icmp/IcmpModule.h"

namespace Util {
namespace Network {
class Datagram;

namespace Ip4 {
class Ip4Address;
}  // namespace Ip4
}  // namespace Network
}  // namespace Util

namespace Kernel::Network::Icmp {

IcmpSocket::IcmpSocket() : DatagramSocket(System::getService<NetworkService>().getNetworkStack().getIcmpModule()) {}

IcmpSocket::~IcmpSocket() {
    auto &icmpModule = Kernel::System::getService<Kernel::NetworkService>().getNetworkStack().getIcmpModule();
    icmpModule.deregisterSocket(*this);
}

bool IcmpSocket::send(const Util::Network::Datagram &datagram) {
    const auto &icmpDatagram = reinterpret_cast<const Util::Network::Icmp::IcmpDatagram&>(datagram);
    const auto &sourceAddress = reinterpret_cast<const Util::Network::Ip4::Ip4Address&>(*bindAddress);
    const auto &destinationAddress = reinterpret_cast<const Util::Network::Ip4::Ip4Address&>(icmpDatagram.getRemoteAddress());
    IcmpModule::writePacket(icmpDatagram.getType(), icmpDatagram.getCode(), sourceAddress, destinationAddress, icmpDatagram.getData(), icmpDatagram.getLength());
    return true;
}

}