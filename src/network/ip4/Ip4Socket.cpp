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

#include "Ip4Socket.h"
#include "kernel/system/System.h"
#include "kernel/service/NetworkService.h"
#include "Ip4Datagram.h"

namespace Network::Ip4 {

void Ip4Socket::send(const Network::Datagram &datagram) {
    auto packet = Util::Stream::ByteArrayOutputStream();
    auto &interface = Ip4Module::writeHeader(packet, reinterpret_cast<const Ip4Address&>(datagram.getRemoteAddress()), reinterpret_cast<const Ip4Datagram&>(datagram).getProtocol(), datagram.getLength());
    packet.write(datagram.getBuffer(), 0, datagram.getLength());
    Ethernet::EthernetModule::finalizePacket(packet);
    interface.getDevice().sendPacket(packet.getBuffer(), packet.getPosition());
}

void Ip4Socket::performBind() {
    if (bindAddress->getType() != NetworkAddress::IP4) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Ip4Socket: Invalid bind address!");
    }

    auto &ip4Module = Kernel::System::getService<Kernel::NetworkService>().getNetworkStack().getIp4Module();
    ip4Module.registerSocket(*this);
}

}