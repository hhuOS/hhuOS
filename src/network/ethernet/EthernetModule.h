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

#ifndef HHUOS_ETHERNETMODULE_H
#define HHUOS_ETHERNETMODULE_H

#include "network/NetworkModule.h"
#include "kernel/log/Logger.h"
#include "lib/util/stream/ByteArrayOutputStream.h"
#include "EthernetHeader.h"

namespace Network::Ethernet {

class EthernetModule : public NetworkModule{

public:
    /**
     * Default Constructor.
     */
    EthernetModule() = default;

    /**
     * Copy Constructor.
     */
    EthernetModule(const EthernetModule &other) = delete;

    /**
     * Assignment operator.
     */
    EthernetModule &operator=(const EthernetModule &other) = delete;

    /**
     * Destructor.
     */
    ~EthernetModule() = default;

    static bool checkPacket(const uint8_t *packet, uint32_t length);

    void readPacket(Util::Stream::ByteArrayInputStream &stream, LayerInformation information, Device::Network::NetworkDevice &device) override;

    static void writeHeader(Util::Stream::OutputStream &stream, Device::Network::NetworkDevice &device, const MacAddress &destinationAddress, EthernetHeader::EtherType etherType);

    static void finalizePacket(Util::Stream::ByteArrayOutputStream &packet);

private:

    static uint32_t calculateCheckSequence(const uint8_t *packet, uint32_t length);

    static Kernel::Logger log;

    static const constexpr uint32_t MINIMUM_PACKET_SIZE = 64;
};

}

#endif
