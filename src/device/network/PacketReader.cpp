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
 */


#include "kernel/service/NetworkService.h"
#include "NetworkDevice.h"
#include "PacketReader.h"
#include "lib/util/io/stream/ByteArrayInputStream.h"
#include "lib/util/network/MacAddress.h"
#include "kernel/network/NetworkStack.h"
#include "kernel/network/ethernet/EthernetModule.h"
#include "kernel/network/NetworkModule.h"
#include "kernel/service/Service.h"

namespace Device::Network {

PacketReader::PacketReader(Device::Network::NetworkDevice &networkDevice) : networkDevice(networkDevice) {}

void PacketReader::run() {
    auto &ethernetModule = Kernel::Service::getService<Kernel::NetworkService>().getNetworkStack().getEthernetModule();

    while (true) {
        const auto &packet = networkDevice.getNextIncomingPacket();
        auto stream = Util::Io::ByteArrayInputStream(packet.buffer, packet.length);
        ethernetModule.readPacket(stream, Kernel::Network::NetworkModule::LayerInformation{Util::Network::MacAddress(), Util::Network::MacAddress(), packet.length}, networkDevice);
        networkDevice.freePacketBuffer(packet.buffer);
    }
}

}