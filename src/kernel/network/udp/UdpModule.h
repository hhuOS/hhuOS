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

#ifndef HHUOS_UDPMODULE_H
#define HHUOS_UDPMODULE_H

#include <cstdint>

#include "kernel/network/NetworkModule.h"

namespace Device {
namespace Network {
class NetworkDevice;
}  // namespace Network
}  // namespace Device
namespace Kernel {
class Logger;
}  // namespace Kernel
namespace Kernel::Network {
class Socket;
}  // namespace Network
namespace Util {
namespace Network {
namespace Ip4 {
class Ip4Address;
class Ip4PortAddress;
}  // namespace Ip4
}  // namespace Network

namespace Stream {
class ByteArrayInputStream;
}  // namespace Stream
}  // namespace Util

namespace Kernel::Network::Udp {

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

    static void writePacket(const Util::Network::Ip4::Ip4PortAddress &sourceAddress, const Util::Network::Ip4::Ip4PortAddress &destinationAddress, const uint8_t *buffer, uint16_t length);

    static uint16_t calculateChecksum(const uint8_t *pseudoHeader, const uint8_t *datagram, uint16_t datagramLength);

private:

    uint16_t generatePort(const Util::Network::Ip4::Ip4Address &address);

    static Kernel::Logger log;
};

}

#endif
