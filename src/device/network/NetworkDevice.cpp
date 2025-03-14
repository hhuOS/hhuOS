/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "NetworkDevice.h"

#include "kernel/service/ProcessService.h"
#include "lib/util/async/Thread.h"
#include "device/network/PacketReader.h"
#include "kernel/process/Process.h"
#include "kernel/memory/VirtualAddressSpace.h"
#include "lib/util/base/HeapMemoryManager.h"
#include "kernel/process/Thread.h"
#include "lib/util/base/Address.h"
#include "kernel/network/ethernet/EthernetModule.h"
#include "kernel/service/Service.h"
#include "lib/util/base/Exception.h"
#include "kernel/process/Scheduler.h"
#include "kernel/service/MemoryService.h"
#include "lib/util/base/Constants.h"
#include "kernel/memory/BitmapMemoryManager.h"

namespace Device::Network {

NetworkDevice::NetworkDevice() :
        incomingPacketMemoryManager(*createPacketManager(MAX_BUFFERED_PACKETS)),
        incomingPacketQueue(MAX_BUFFERED_PACKETS),
        outgoingPacketQueue(MAX_BUFFERED_PACKETS),
        reader(new PacketReader(*this)) {
    auto &processService = Kernel::Service::getService<Kernel::ProcessService>();
    auto &readerThread = Kernel::Thread::createKernelThread("Packet-Reader", processService.getKernelProcess(), reader);

    processService.getScheduler().ready(readerThread);
}

NetworkDevice::~NetworkDevice() {
    delete &incomingPacketMemoryManager;
}

void NetworkDevice::setIdentifier(const Util::String &identifier) {
    NetworkDevice::identifier = identifier;
}

const Util::String& NetworkDevice::getIdentifier() const {
    return identifier;
}

void NetworkDevice::sendPacket(const uint8_t *packet, uint32_t length) {
    if (length > MAX_ETHERNET_PACKET_SIZE) {
        return; // Discard too large packets
    }

    // Allocate kernel buffer to copy and send packet
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
    auto *buffer = static_cast<uint8_t*>(memoryService.mapIO(1));

    // Copy packet into kernel buffer
    auto source = Util::Address(packet);
    auto target = Util::Address(buffer);
    target.copyRange(source, length);

    // Add padding, if necessary
    if (length < MIN_ETHERNET_PACKET_SIZE) {
        target.add(length).setRange(0, MIN_ETHERNET_PACKET_SIZE - length);
        length = MIN_ETHERNET_PACKET_SIZE;
    }

    outgoingPacketLock.acquire();
    outgoingPacketQueue.add(Packet{buffer, length});
    handleOutgoingPacket(buffer, length);
    outgoingPacketLock.release();
}

void NetworkDevice::handleIncomingPacket(const uint8_t *packet, uint32_t length) {
    if (!Kernel::Network::Ethernet::EthernetModule::checkPacket(packet, length)) {
        return; // Discard packets failing the checksum test
    }

    if (length > incomingPacketMemoryManager.getBlockSize()) {
        return; // Discard too large packets
    }

    auto *buffer = reinterpret_cast<uint8_t*>(incomingPacketMemoryManager.allocateBlock());
    if (buffer == nullptr) {
        return; // No packet memory available -> Discard packet
    }

    auto source = Util::Address(packet);
    auto target = Util::Address(buffer);
    target.copyRange(source, length);

    if (!incomingPacketQueue.offer(Packet{buffer, length})) {
        incomingPacketMemoryManager.freeBlock(buffer);
    }
}

NetworkDevice::Packet NetworkDevice::getNextIncomingPacket() {
    while (incomingPacketQueue.isEmpty()) {
        Util::Async::Thread::yield();
    }

    return incomingPacketQueue.poll();
}

NetworkDevice::Packet NetworkDevice::getNextOutgoingPacket() {
    while (outgoingPacketQueue.isEmpty()) {
        Util::Async::Thread::yield();
    }

    return outgoingPacketQueue.poll();
}

void NetworkDevice::freePacketBuffer(void *buffer) {
    if (buffer >= incomingPacketMemoryManager.getStartAddress() && buffer <= incomingPacketMemoryManager.getEndAddress()) {
        incomingPacketMemoryManager.freeBlock(buffer);
    } else {
        Util::Exception::throwException(Util::Exception::OUT_OF_BOUNDS, "NetworkDevice: Trying to free an invalid packet buffer!");
    }
}

void NetworkDevice::freeLastSendBuffer() {
    outgoingPacketsToFree++;

    auto &kernelProcess = Kernel::Service::getService<Kernel::ProcessService>().getKernelProcess();
    if (!kernelProcess.getAddressSpace().getMemoryManager().isLocked()) {
        while (outgoingPacketsToFree > 0) {
            const auto &packet = outgoingPacketQueue.poll();
            delete packet.buffer;
            outgoingPacketsToFree--;
        }
    }
}

Kernel::BitmapMemoryManager* NetworkDevice::createPacketManager(uint32_t packetCount) {
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
    auto *startAddress = static_cast<uint8_t*>(memoryService.mapIO(packetCount / PACKETS_PER_PAGE));
    auto *endAddress = startAddress + (packetCount / PACKETS_PER_PAGE) * Util::PAGESIZE - 1;

    return new Kernel::BitmapMemoryManager(startAddress, endAddress);
}

bool NetworkDevice::Packet::operator==(const NetworkDevice::Packet &other) const {
    return buffer == other.buffer;
}

}