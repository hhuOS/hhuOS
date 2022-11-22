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

#ifndef HHUOS_NETWORKDEVICE_H
#define HHUOS_NETWORKDEVICE_H

#include "kernel/interrupt/InterruptHandler.h"
#include "kernel/memory/BitmapMemoryManager.h"
#include "lib/util/stream/FilterInputStream.h"
#include "lib/util/stream/OutputStream.h"
#include "lib/util/stream/PipedOutputStream.h"
#include "lib/util/stream/ByteArrayInputStream.h"
#include "lib/util/data/ArrayBlockingQueue.h"
#include "network/MacAddress.h"
#include "kernel/log/Logger.h"
#include "PacketReader.h"
#include "PacketWriter.h"

namespace Device::Network {

/**
 * Interface for network cards
 */
class NetworkDevice {

friend class PacketReader;
friend class PacketWriter;

public:
    /**
     * Default Constructor.
     */
    explicit NetworkDevice(const Util::Memory::String &identifier);

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

    Util::Memory::String getIdentifier() const;

    [[nodiscard]] virtual ::Network::MacAddress getMacAddress() const = 0;

    void sendPacket(const uint8_t *packet, uint32_t length);

    Util::Stream::ByteArrayInputStream* getNextIncomingPacket();

    Util::Stream::ByteArrayInputStream* getNextOutgoingPacket();

protected:

    virtual void handleOutgoingPacket(const uint8_t *packet, uint32_t length) = 0;

    void handleIncomingPacket(const uint8_t *packet, uint32_t length);

private:

    void freePacketBuffer(void *buffer);

    Util::Memory::String identifier;
    Kernel::Logger log;

    uint8_t *packetMemory;
    Kernel::BitmapMemoryManager packetMemoryManager;
    Util::Data::ArrayBlockingQueue<Util::Stream::ByteArrayInputStream*> incomingPacketQueue;
    Util::Data::ArrayBlockingQueue<Util::Stream::ByteArrayInputStream*> outgoingPacketQueue;

    PacketReader *reader;
    PacketWriter *writer;

    static const constexpr uint32_t PACKET_BUFFER_SIZE = 2048;
    static const constexpr uint32_t MAX_BUFFERED_PACKETS = 16;
};

}

#endif