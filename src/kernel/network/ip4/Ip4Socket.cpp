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

#include "Ip4Socket.h"

#include "kernel/system/System.h"
#include "kernel/service/NetworkService.h"
#include "lib/util/network/ip4/Ip4Datagram.h"
#include "device/network/NetworkDevice.h"
#include "lib/util/stream/ByteArrayOutputStream.h"
#include "lib/util/network/Datagram.h"
#include "kernel/network/NetworkStack.h"
#include "kernel/network/ethernet/EthernetModule.h"
#include "kernel/network/ip4/Ip4Interface.h"
#include "kernel/network/ip4/Ip4Module.h"

namespace Util {
namespace Network {
namespace Ip4 {
class Ip4Address;
}  // namespace Ip4
}  // namespace Network
}  // namespace Util

namespace Kernel::Network::Ip4 {

Ip4Socket::Ip4Socket() : DatagramSocket(System::getService<NetworkService>().getNetworkStack().getIp4Module()) {}

Ip4Socket::~Ip4Socket() {
    auto &ip4Module = Kernel::System::getService<Kernel::NetworkService>().getNetworkStack().getIp4Module();
    ip4Module.deregisterSocket(*this);
}

bool Ip4Socket::send(const Util::Network::Datagram &datagram) {
    auto packet = Util::Stream::ByteArrayOutputStream();
    const auto &ip4Datagram = reinterpret_cast<const Util::Network::Ip4::Ip4Datagram&>(datagram);
    const auto &sourceAddress = reinterpret_cast<const Util::Network::Ip4::Ip4Address&>(*bindAddress);
    const auto &destinationAddress = reinterpret_cast<const Util::Network::Ip4::Ip4Address&>(ip4Datagram.getRemoteAddress());
    auto &interface = Ip4Module::writeHeader(packet, sourceAddress, destinationAddress, ip4Datagram.getProtocol(), datagram.getLength());
    packet.write(datagram.getData(), 0, datagram.getLength());
    Ethernet::EthernetModule::finalizePacket(packet);
    interface.getDevice().sendPacket(packet.getBuffer(), packet.getPosition());
    return true;
}

}