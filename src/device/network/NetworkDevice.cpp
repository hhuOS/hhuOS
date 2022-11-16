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

#include "NetworkDevice.h"
#include "kernel/system/System.h"
#include "kernel/service/NetworkService.h"

namespace Device::Network {

Kernel::Logger NetworkDevice::log = Kernel::Logger::get("Network");

NetworkDevice::NetworkDevice() :
        packetMemory(static_cast<uint8_t *>(Kernel::System::getService<Kernel::MemoryService>().allocateKernelMemory(MAX_BUFFERED_PACKETS * PACKET_BUFFER_SIZE, Util::Memory::PAGESIZE))),
        packetMemoryManager(reinterpret_cast<uint32_t>(packetMemory), reinterpret_cast<uint32_t>(packetMemory + MAX_BUFFERED_PACKETS * PACKET_BUFFER_SIZE - 1), PACKET_BUFFER_SIZE),
        packetQueue(PACKET_BUFFER_SIZE) {}

void NetworkDevice::handlePacket(const uint8_t *packet, uint32_t length) {
    auto &ethernetModule = Kernel::System::getService<Kernel::NetworkService>().getEthernetModule();
    if (ethernetModule.checkPacket(packet, length)) {
        // Do not write checksum, since it has already been handled by checkPacket()
        auto *buffer = reinterpret_cast<uint8_t*>(packetMemoryManager.allocateBlock());
        auto *stream = new Util::Stream::ByteArrayInputStream(buffer, length);

        auto source = Util::Memory::Address<uint32_t>(packet);
        auto target = Util::Memory::Address<uint32_t>(buffer);
        target.copyRange(source, length);

        if (!packetQueue.offer(stream)) {
            log.warn("Dropping packet, because of too many unhandled packets");
            delete stream;
            delete buffer;
        }
    }
}

NetworkDevice::~NetworkDevice() {
    delete[] packetMemory;
    for (const auto *stream : packetQueue) {
        delete stream;
    }
}

Util::Stream::InputStream* NetworkDevice::getNextPacket() {
    return packetQueue.poll();
}

}