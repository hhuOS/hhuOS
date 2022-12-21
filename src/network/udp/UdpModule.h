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
 */

#ifndef HHUOS_UDPMODULE_H
#define HHUOS_UDPMODULE_H

#include <cstdint>

#include "network/NetworkModule.h"

namespace Device {
namespace Network {
class NetworkDevice;
}  // namespace Network
}  // namespace Device
namespace Kernel {
class Logger;
}  // namespace Kernel
namespace Network {
class Socket;
}  // namespace Network
namespace Util {
namespace Network {
namespace Ip4 {
class Ip4Address;
}  // namespace Ip4
}  // namespace Network

namespace Stream {
class ByteArrayInputStream;
}  // namespace Stream
}  // namespace Util

namespace Network::Udp {

class UdpModule : public NetworkModule {

public:
    /**
     * Default Constructor.
     */
    UdpModule() = default;

    /**
     * Copy Constructor.
     */
    UdpModule(const UdpModule &other) = delete;

    /**
     * Assignment operator.
     */
    UdpModule &operator=(const UdpModule &other) = delete;

    /**
     * Destructor.
     */
    ~UdpModule() = default;

    virtual bool registerSocket(Socket &socket) override;

    void readPacket(Util::Stream::ByteArrayInputStream &stream, LayerInformation information, Device::Network::NetworkDevice &device) override;

    static void writePacket(uint16_t sourcePort, uint16_t destinationPort, const Util::Network::Ip4::Ip4Address &destinationAddress, const uint8_t *buffer, uint16_t length);

    static uint16_t calculateChecksum(const uint8_t *pseudoHeader, const uint8_t *datagram, uint16_t datagramLength);

private:

    static Kernel::Logger log;
};

}

#endif
