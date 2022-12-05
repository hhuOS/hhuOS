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
#include "lib/util/async/Thread.h"

namespace Network::Udp {

UdpSocket::UdpSocket(uint16_t port) : port(port) {
    auto &udpModule = Kernel::System::getService<Kernel::NetworkService>().getNetworkStack().getUdpModule();
    if (!udpModule.registerSocket(*this)) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Port is already in use!");
    }
}

void UdpSocket::send(const UdpDatagram &datagram) const {
    Network::Udp::UdpModule::writePacket(port, datagram.getRemotePort(), reinterpret_cast<const Ip4::Ip4Address&>(datagram.getRemoteAddress()), datagram.getBuffer(), datagram.getLength());
}

UdpDatagram UdpSocket::receive() {
    while (incomingDatagramQueue.isEmpty()) {
        Util::Async::Thread::yield();
    }

    lock.acquire();
    return lock.releaseAndReturn(incomingDatagramQueue.poll());
}

uint16_t UdpSocket::getPort() const {
    return port;
}

void UdpSocket::handleIncomingDatagram(const UdpDatagram &datagram) {
    lock.acquire();
    incomingDatagramQueue.offer(datagram);
    lock.release();
}

}