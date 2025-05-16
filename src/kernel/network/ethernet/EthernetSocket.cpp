/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include "EthernetSocket.h"

#include "kernel/service/NetworkService.h"
#include "lib/util/network/ethernet/EthernetDatagram.h"
#include "device/network/NetworkDevice.h"
#include "lib/util/io/stream/ByteArrayOutputStream.h"
#include "lib/util/network/Datagram.h"
#include "kernel/network/NetworkStack.h"
#include "kernel/network/ethernet/EthernetModule.h"
#include "lib/util/network/Socket.h"
#include "kernel/service/Service.h"

namespace Util {
namespace Network {
class MacAddress;
}  // namespace Network
}  // namespace Util

namespace Kernel::Network::Ethernet {

EthernetSocket::EthernetSocket() : DatagramSocket(Service::getService<NetworkService>().getNetworkStack().getEthernetModule(), Util::Network::Socket::ETHERNET) {}

EthernetSocket::~EthernetSocket() {
    auto &ethernetModule = Service::getService<NetworkService>().getNetworkStack().getEthernetModule();
    ethernetModule.deregisterSocket(*this);
}

bool EthernetSocket::send(const Util::Network::Datagram &datagram) {
    auto &networkService = Service::getService<NetworkService>();
    auto &device = networkService.getNetworkDevice(reinterpret_cast<const Util::Network::MacAddress&>(getAddress()));
    auto packet = Util::Io::ByteArrayOutputStream();

    EthernetModule::writeHeader(packet, device, reinterpret_cast<const Util::Network::MacAddress &>(datagram.getRemoteAddress()), reinterpret_cast<const Util::Network::Ethernet::EthernetDatagram&>(datagram).getEtherType());
    packet.write(datagram.getData(), 0, datagram.getLength());
    EthernetModule::finalizePacket(packet);

    device.sendPacket(packet.getBuffer(), packet.getPosition());
    return true;
}

}