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
#include "kernel/service/ProcessService.h"
#include "lib/util/async/Thread.h"

namespace Device::Network {

NetworkDevice::NetworkDevice(const Util::Memory::String &identifier) :
        identifier(identifier),
        packetMemory(static_cast<uint8_t *>(Kernel::System::getService<Kernel::MemoryService>().allocateKernelMemory(MAX_BUFFERED_PACKETS * PACKET_BUFFER_SIZE, Util::Memory::PAGESIZE))),
        packetMemoryManager(reinterpret_cast<uint32_t>(packetMemory), reinterpret_cast<uint32_t>(packetMemory + MAX_BUFFERED_PACKETS * PACKET_BUFFER_SIZE - 1), PACKET_BUFFER_SIZE),
        incomingPacketQueue(MAX_BUFFERED_PACKETS),
        outgoingPacketQueue(MAX_BUFFERED_PACKETS),
        reader(new PacketReader(*this)),
        writer(new PacketWriter(*this)),
        log(Kernel::Logger::get(identifier)) {
    auto &schedulerService = Kernel::System::getService<Kernel::SchedulerService>();
    auto &processService = Kernel::System::getService<Kernel::ProcessService>();
    auto &readerThread = Kernel::Thread::createKernelThread("Loopback-Reader", processService.getKernelProcess(), reader);
    auto &writerThread = Kernel::Thread::createKernelThread("Loopback-Writer", processService.getKernelProcess(), writer);

    schedulerService.ready(readerThread);
    schedulerService.ready(writerThread);
}

Util::Memory::String NetworkDevice::getIdentifier() const {
    return identifier;
}

void NetworkDevice::sendPacket(const uint8_t *packet, uint32_t length) {
    outgoingPacketLock.acquire();
    auto *buffer = reinterpret_cast<uint8_t*>(packetMemoryManager.allocateBlock());
    auto source = Util::Memory::Address<uint32_t>(packet);
    auto target = Util::Memory::Address<uint32_t>(buffer);
    target.copyRange(source, length);

    outgoingPacketQueue.add({buffer, length});
    outgoingPacketLock.release();
}

void NetworkDevice::handleIncomingPacket(const uint8_t *packet, uint32_t length) {
    if (!::Network::Ethernet::EthernetModule::checkPacket(packet, length)) {
        log.warn("Discarding packet because of not matching frame check sequence");
        return;
    }

    auto *buffer = reinterpret_cast<uint8_t*>(packetMemoryManager.allocateBlock());
    auto source = Util::Memory::Address<uint32_t>(packet);
    auto target = Util::Memory::Address<uint32_t>(buffer);
    target.copyRange(source, length);

    if (!incomingPacketQueue.offer({buffer, length})) {
        log.warn("Discarding packet, because of too many unhandled packets");
        packetMemoryManager.freeBlock(buffer);
    }
}

NetworkDevice::~NetworkDevice() {
    delete[] packetMemory;

    for (const auto *address : addressList) {
        delete address;
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
    packetMemoryManager.freeBlock(buffer);
}

void NetworkDevice::addAddress(const ::Network::NetworkAddress &address) {
    addressLock.acquire();
    if (!hasAddress(address)) {
        addressList.add(address.createCopy());
    }
    addressLock.release();
}

void NetworkDevice::removeAddress(const ::Network::NetworkAddress &address) {
    addressLock.acquire();
    for (uint32_t i = 0; i < addressList.size(); i++) {
        if (*addressList.get(i) == address) {
            delete addressList.removeIndex(i);
            addressLock.release();
            return;
        }
    }
    addressLock.release();
}

bool NetworkDevice::hasAddress(const ::Network::NetworkAddress &address) {
    addressLock.acquire();
    for (const auto *currentAddress : addressList) {
        if (*currentAddress == address) {
            addressLock.release();
            return true;
        }
    }

    addressLock.release();
    return false;
}

bool NetworkDevice::Packet::operator==(const NetworkDevice::Packet &other) const {
    return buffer == other.buffer;
}

}