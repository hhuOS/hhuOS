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

#include "EthernetSocket.h"
#include "kernel/system/System.h"
#include "kernel/service/NetworkService.h"
#include "EthernetDatagram.h"

namespace Network::Ethernet {

EthernetSocket::~EthernetSocket() {
    auto &ethernetModule = Kernel::System::getService<Kernel::NetworkService>().getNetworkStack().getEthernetModule();
    ethernetModule.deregisterSocket(*this);
}

void EthernetSocket::send(const Datagram &datagram) {
    auto packet = Util::Stream::ByteArrayOutputStream();
    EthernetModule::writeHeader(packet, *device, reinterpret_cast<const MacAddress&>(datagram.getRemoteAddress()), reinterpret_cast<const EthernetDatagram&>(datagram).getEtherType());
    packet.write(datagram.getData(), 0, datagram.getDataLength());
    EthernetModule::finalizePacket(packet);
    device->sendPacket(packet.getBuffer(), packet.getPosition());
}

void EthernetSocket::performBind() {
    if (bindAddress->getType() != NetworkAddress::MAC) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "EthernetSocket: Invalid bind address!");
    }

    auto &networkService = Kernel::System::getService<Kernel::NetworkService>();
    device = &networkService.getNetworkDevice(reinterpret_cast<const MacAddress&>(*bindAddress));

    auto &ethernetModule = networkService.getNetworkStack().getEthernetModule();
    if (!ethernetModule.registerSocket(*this)) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Failed to register Ethernet socket!");
    }
}

}