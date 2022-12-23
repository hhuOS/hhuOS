/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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
 *
 * The UDP/IP stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#ifndef HHUOS_ETHERNETMODULE_H
#define HHUOS_ETHERNETMODULE_H

#include <cstdint>

#include "kernel/network/NetworkModule.h"
#include "lib/util/network/ethernet/EthernetHeader.h"

namespace Device {
namespace Network {
class NetworkDevice;
}  // namespace Network
}  // namespace Device
namespace Kernel {
class Logger;
}  // namespace Kernel
namespace Util {
namespace Network {
class MacAddress;
}  // namespace Network

namespace Stream {
class ByteArrayInputStream;
class ByteArrayOutputStream;
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Kernel::Network::Ethernet {

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

    static void writeHeader(Util::Stream::OutputStream &stream, Device::Network::NetworkDevice &device, const Util::Network::MacAddress &destinationAddress, Util::Network::Ethernet::EthernetHeader::EtherType etherType);

    static void finalizePacket(Util::Stream::ByteArrayOutputStream &packet);

private:

    static uint32_t calculateCheckSequence(const uint8_t *packet, uint32_t length);

    static Kernel::Logger log;

    static const constexpr uint32_t MINIMUM_PACKET_SIZE = 64;
};

}

#endif
