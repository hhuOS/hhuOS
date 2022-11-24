/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Hannes Feil, Michael Schoettner
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

#include "network/ethernet/EthernetHeader.h"
#include "kernel/system/System.h"
#include "kernel/service/NetworkService.h"
#include "NetworkDevice.h"
#include "PacketReader.h"

namespace Device::Network {

PacketReader::PacketReader(Device::Network::NetworkDevice &networkDevice) : networkDevice(networkDevice) {}

void PacketReader::run() {
    auto &ethernetModule = Kernel::System::getService<Kernel::NetworkService>().getNetworkStack().getEthernetModule();

    while (true) {
        const auto &packet = networkDevice.getNextIncomingPacket();
        auto stream = Util::Stream::ByteArrayInputStream(packet.buffer, packet.length, false);
        ethernetModule.readPacket(stream, {::Network::MacAddress(), ::Network::MacAddress(), packet.length}, networkDevice);
        networkDevice.freePacketBuffer(packet.buffer);
    }
}

}