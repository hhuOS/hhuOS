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

#ifndef HHUOS_PACKETWRITER_H
#define HHUOS_PACKETWRITER_H

#include "lib/util/async/Runnable.h"
#include "device/network/NetworkDevice.h"
#include "lib/util/collection/Array.h"
#include "lib/util/collection/ArrayBlockingQueue.h"
#include "lib/util/collection/Collection.h"
#include "lib/util/collection/Iterator.h"

namespace Device::Network {

class PacketWriter : public Util::Async::Runnable {

public:
    /**
     * Default Constructor.
     */
    PacketWriter(NetworkDevice &networkDevice);

    /**
     * Copy Constructor.
     */
    PacketWriter(const PacketWriter &other) = delete;

    /**
     * Assignment operator.
     */
    PacketWriter &operator=(const PacketWriter &other) = delete;

    /**
     * Destructor.
     */
    ~PacketWriter() override = default;

    void run() override;

    void freeLastSendBuffer();

private:

    Device::Network::NetworkDevice &networkDevice;
    Util::ArrayBlockingQueue<NetworkDevice::Packet> packetQueue = Util::ArrayBlockingQueue<NetworkDevice::Packet>(16);
};

}

#endif
