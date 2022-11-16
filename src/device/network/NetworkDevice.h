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

namespace Device::Network {

/**
 * Interface for network cards
 */
class NetworkDevice {

public:
    /**
     * Default Constructor.
     */
    NetworkDevice();

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

    /**
     * Read the MAC-address into a given buffer.
     *
     * @param buf The buffer to read the MAC-address into.
     */
    virtual ::Network::MacAddress getMacAddress() = 0;

    virtual void sendPacket(const uint8_t *packet, uint32_t length) = 0;

    Util::Stream::InputStream* getNextPacket();

protected:

    void handlePacket(const uint8_t *packet, uint32_t length);

private:

    uint8_t *packetMemory;
    Kernel::BitmapMemoryManager packetMemoryManager;
    Util::Data::ArrayBlockingQueue<Util::Stream::ByteArrayInputStream*> packetQueue;

    static Kernel::Logger log;

    static const constexpr uint32_t PACKET_BUFFER_SIZE = 2048;
    static const constexpr uint32_t MAX_BUFFERED_PACKETS = 16;
};

}

#endif