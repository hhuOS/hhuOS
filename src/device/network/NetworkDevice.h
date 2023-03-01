/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_NETWORKDEVICE_H
#define HHUOS_NETWORKDEVICE_H

#include <cstdint>

#include "kernel/memory/BitmapMemoryManager.h"
#include "lib/util/collection/ArrayBlockingQueue.h"
#include "lib/util/network/MacAddress.h"
#include "kernel/log/Logger.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/collection/Array.h"
#include "lib/util/collection/Collection.h"
#include "lib/util/collection/Iterator.h"
#include "lib/util/base/String.h"

namespace Device {
namespace Network {
class PacketReader;
class PacketWriter;
}  // namespace Network
}  // namespace Device

namespace Kernel {
class NetworkService;
}  // namespace Kernel

namespace Device::Network {

/**
 * Interface for network cards
 */
class NetworkDevice {

friend class PacketReader;
friend class PacketWriter;
friend class Kernel::NetworkService;

public:

    struct Packet {
        uint8_t *buffer;
        uint32_t length;

        bool operator==(const Packet &other) const;
    };

    /**
     * Default Constructor.
     */
    explicit NetworkDevice();

    /**
     * Copy-constructor.
     */
    NetworkDevice(const NetworkDevice &copy) = delete;

    /**
     * Assignment operator.
     */
    NetworkDevice &operator=(const NetworkDevice &other) = delete;

    /**
     * Destructor.
     */
    virtual ~NetworkDevice();

    [[nodiscard]] const Util::String& getIdentifier() const;

    [[nodiscard]] virtual Util::Network::MacAddress getMacAddress() const = 0;

    void sendPacket(const uint8_t *packet, uint32_t length);

    Packet getNextIncomingPacket();

    Packet getNextOutgoingPacket();

protected:

    virtual void handleOutgoingPacket(const uint8_t *packet, uint32_t length) = 0;

    void handleIncomingPacket(const uint8_t *packet, uint32_t length);

    void freeLastSendBuffer();

private:

    void freePacketBuffer(void *buffer);

    Util::String identifier;

    uint8_t *packetMemory;
    Kernel::BitmapMemoryManager packetMemoryManager;
    Util::ArrayBlockingQueue<Packet> incomingPacketQueue;
    Util::ArrayBlockingQueue<Packet> outgoingPacketQueue;
    Util::Async::Spinlock outgoingPacketLock;

    PacketReader *reader;
    PacketWriter *writer;

    Kernel::Logger log;

    static const constexpr uint32_t PACKET_BUFFER_SIZE = 2048;
    static const constexpr uint32_t MAX_BUFFERED_PACKETS = 16;
};

}

#endif