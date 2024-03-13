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
 
 #include "NetworkPacketMemoryManager.h"

#include <kernel/service/MemoryService.h>
#include <kernel/system/System.h>
#include <lib/util/base/Constants.h>

NetworkPacketMemoryManager NetworkPacketMemoryManager::create(uint32_t packetCount) {
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
    auto *startAddress = static_cast<uint8_t*>(memoryService.allocateKernelMemory(packetCount * PACKET_BUFFER_SIZE, Util::PAGESIZE));
    auto *endAddress = startAddress + packetCount * PACKET_BUFFER_SIZE - 1;

    return NetworkPacketMemoryManager(startAddress, endAddress);
}

NetworkPacketMemoryManager::NetworkPacketMemoryManager(uint8_t* startAddress, uint8_t* endAddress) : BitmapMemoryManager(startAddress, endAddress, PACKET_BUFFER_SIZE) {}

NetworkPacketMemoryManager::~NetworkPacketMemoryManager() {
    delete buffer;
}

void* NetworkPacketMemoryManager::allocateBlock() {
    auto *buffer = BitmapMemoryManager::allocateBlock();
    while (buffer == nullptr) {
        buffer = BitmapMemoryManager::allocateBlock();
    }

    return buffer;
}

void NetworkPacketMemoryManager::handleError() {}
