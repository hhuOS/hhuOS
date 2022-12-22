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

#include "EthernetSocket.h"

#include "kernel/system/System.h"
#include "kernel/service/NetworkService.h"
#include "lib/util/network/ethernet/EthernetDatagram.h"
#include "device/network/NetworkDevice.h"
#include "lib/util/Exception.h"
#include "lib/util/stream/ByteArrayOutputStream.h"
#include "lib/util/network/Datagram.h"
#include "lib/util/network/NetworkAddress.h"
#include "network/NetworkStack.h"
#include "network/ethernet/EthernetModule.h"

namespace Util {
namespace Network {
class MacAddress;
}  // namespace Network
}  // namespace Util

namespace Network::Ethernet {

EthernetSocket::~EthernetSocket() {
    auto &ethernetModule = Kernel::System::getService<Kernel::NetworkService>().getNetworkStack().getEthernetModule();
    ethernetModule.deregisterSocket(*this);
}

bool EthernetSocket::send(const Util::Network::Datagram &datagram) {
    auto packet = Util::Stream::ByteArrayOutputStream();
    EthernetModule::writeHeader(packet, *device, reinterpret_cast<const Util::Network::MacAddress &>(datagram.getRemoteAddress()),
                                reinterpret_cast<const Util::Network::Ethernet::EthernetDatagram &>(datagram).getEtherType());
    packet.write(datagram.getData(), 0, datagram.getDataLength());
    EthernetModule::finalizePacket(packet);
    device->sendPacket(packet.getBuffer(), packet.getPosition());
    return true;
}

void EthernetSocket::performBind() {
    if (bindAddress->getType() != Util::Network::NetworkAddress::MAC) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "EthernetSocket: Invalid bind address!");
    }

    auto &networkService = Kernel::System::getService<Kernel::NetworkService>();
    device = &networkService.getNetworkDevice(reinterpret_cast<const Util::Network::MacAddress &>(*bindAddress));

    auto &ethernetModule = networkService.getNetworkStack().getEthernetModule();
    if (!ethernetModule.registerSocket(*this)) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Failed to register Ethernet socket!");
    }
}

}