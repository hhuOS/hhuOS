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

#include "UdpSocket.h"

#include "UdpModule.h"
#include "kernel/system/System.h"
#include "kernel/service/NetworkService.h"
#include "lib/util/Exception.h"
#include "network/Datagram.h"
#include "lib/util/network/NetworkAddress.h"
#include "network/NetworkStack.h"
#include "lib/util/network/ip4/Ip4PortAddress.h"

namespace Network::Udp {

UdpSocket::~UdpSocket() {
    auto &udpModule = Kernel::System::getService<Kernel::NetworkService>().getNetworkStack().getUdpModule();
    udpModule.deregisterSocket(*this);
}

void UdpSocket::send(const Datagram &datagram) {
    auto sourcePort = reinterpret_cast<const Util::Network::Ip4::Ip4PortAddress*>(bindAddress)->getPort();
    const auto remoteAddress = reinterpret_cast<const Util::Network::Ip4::Ip4PortAddress&>(datagram.getRemoteAddress());
    UdpModule::writePacket(sourcePort, remoteAddress.getPort(), remoteAddress.getIp4Address(), datagram.getData(),
                           datagram.getDataLength());
}

uint16_t UdpSocket::getPort() const {
    return reinterpret_cast<const Util::Network::Ip4::Ip4PortAddress*>(bindAddress)->getPort();
}

void UdpSocket::performBind() {
    if (bindAddress->getType() != Util::Network::NetworkAddress::IP4_PORT) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "UdpSocket: Invalid bind address!");
    }

    auto &udpModule = Kernel::System::getService<Kernel::NetworkService>().getNetworkStack().getUdpModule();
    if (!udpModule.registerSocket(*this)) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Failed to register UDP socket!");
    }
}

}