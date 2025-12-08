/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include <stdint.h>

#include "kernel/network/NetworkModule.h"
#include "lib/util/network/ethernet/EthernetHeader.h"

namespace Device {
namespace Network {
class NetworkDevice;
}  // namespace Network
}  // namespace Device
namespace Util {
namespace Network {
class MacAddress;
}  // namespace Network

namespace Io {
class ByteArrayOutputStream;
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Kernel {
namespace Network {
namespace Ethernet {

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

    static uint32_t calculateCheckSequence(const uint8_t *packet, uint32_t length);

    void readPacket(Util::Io::ByteArrayInputStream &stream, LayerInformation information, Device::Network::NetworkDevice &device) override;

    static void writeHeader(Util::Io::OutputStream &stream, Device::Network::NetworkDevice &device, const Util::Network::MacAddress &destinationAddress, Util::Network::Ethernet::EthernetHeader::EtherType etherType);

    static void finalizePacket(Util::Io::ByteArrayOutputStream &packet);

private:

    static const constexpr uint32_t MINIMUM_PACKET_SIZE = 64;
};

}
}
}

#endif
