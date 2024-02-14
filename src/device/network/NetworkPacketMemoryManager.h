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
 
#ifndef NETWORKPACKETMEMORYMANAGER_H
#define NETWORKPACKETMEMORYMANAGER_H
#include <kernel/memory/BitmapMemoryManager.h>

class NetworkPacketMemoryManager : public Kernel::BitmapMemoryManager {

public:

    static NetworkPacketMemoryManager create(uint32_t packetCount);

    /**
     * Copy Constructor.
     */
    NetworkPacketMemoryManager(const NetworkPacketMemoryManager &other) = delete;

    /**
     * Assignment operator.
     */
    NetworkPacketMemoryManager &operator=(const NetworkPacketMemoryManager &other) = delete;

    /**
     * Destructor.
     */
    ~NetworkPacketMemoryManager() override;

    void* allocateBlock() override;

    void handleError() override;

private:
    /**
     * Constructor.
     */
    NetworkPacketMemoryManager(uint8_t *startAddress, uint8_t *endAddress);

    uint8_t *buffer;

    static const constexpr uint32_t PACKET_BUFFER_SIZE = 2048;
};

#endif
