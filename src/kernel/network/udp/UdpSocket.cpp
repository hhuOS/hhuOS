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

#include "UdpSocket.h"

#include "UdpModule.h"
#include "kernel/system/System.h"
#include "kernel/service/NetworkService.h"
#include "lib/util/network/Datagram.h"
#include "kernel/network/NetworkStack.h"
#include "lib/util/network/ip4/Ip4PortAddress.h"

namespace Kernel::Network::Udp {

UdpSocket::UdpSocket() : DatagramSocket(System::getService<NetworkService>().getNetworkStack().getUdpModule()) {}

UdpSocket::~UdpSocket() {
    auto &udpModule = Kernel::System::getService<Kernel::NetworkService>().getNetworkStack().getUdpModule();
    udpModule.deregisterSocket(*this);
}

bool UdpSocket::send(const Util::Network::Datagram &datagram) {
    const auto &sourceAddress = reinterpret_cast<const Util::Network::Ip4::Ip4PortAddress&>(*bindAddress);
    const auto &destinationAddress = reinterpret_cast<const Util::Network::Ip4::Ip4PortAddress&>(datagram.getRemoteAddress());
    UdpModule::writePacket(sourceAddress, destinationAddress, datagram.getData(), datagram.getLength());
    return true;
}

uint16_t UdpSocket::getPort() const {
    return reinterpret_cast<const Util::Network::Ip4::Ip4PortAddress*>(bindAddress)->getPort();
}

}