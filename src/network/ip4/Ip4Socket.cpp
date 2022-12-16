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
#include "device/network/NetworkDevice.h"
#include "lib/util/Exception.h"
#include "lib/util/stream/ByteArrayOutputStream.h"
#include "network/Datagram.h"
#include "network/NetworkAddress.h"
#include "network/NetworkStack.h"
#include "network/ethernet/EthernetModule.h"
#include "network/ip4/Ip4Interface.h"
#include "network/ip4/Ip4Module.h"

namespace Network {
namespace Ip4 {
class Ip4Address;
}  // namespace Ip4
}  // namespace Network

namespace Network::Ip4 {

Ip4Socket::~Ip4Socket() {
    auto &ip4Module = Kernel::System::getService<Kernel::NetworkService>().getNetworkStack().getIp4Module();
    ip4Module.deregisterSocket(*this);
}

void Ip4Socket::send(const Network::Datagram &datagram) {
    auto packet = Util::Stream::ByteArrayOutputStream();
    auto &interface = Ip4Module::writeHeader(packet, reinterpret_cast<const Ip4Address&>(datagram.getRemoteAddress()), reinterpret_cast<const Ip4Datagram&>(datagram).getProtocol(),
                                             datagram.getDataLength());
    packet.write(datagram.getData(), 0, datagram.getDataLength());
    Ethernet::EthernetModule::finalizePacket(packet);
    interface.getDevice().sendPacket(packet.getBuffer(), packet.getPosition());
}

void Ip4Socket::performBind() {
    if (bindAddress->getType() != NetworkAddress::IP4) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Ip4Socket: Invalid bind address!");
    }

    auto &ip4Module = Kernel::System::getService<Kernel::NetworkService>().getNetworkStack().getIp4Module();
    if (!ip4Module.registerSocket(*this)) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Failed to register IP4 socket!");
    }
}

}