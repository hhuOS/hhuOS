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

#ifndef HHUOS_ICMPMODULE_H
#define HHUOS_ICMPMODULE_H

#include <cstdint>

#include "kernel/network/NetworkModule.h"
#include "lib/util/network/icmp/IcmpHeader.h"

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
namespace Icmp {
class EchoHeader;
}  // namespace Icmp

namespace Ip4 {
class Ip4Address;
}  // namespace Ip4
}  // namespace Network

namespace Stream {
class ByteArrayInputStream;
}  // namespace Stream
}  // namespace Util

namespace Kernel::Network::Icmp {

class IcmpModule : public NetworkModule {

public:
    /**
     * Default Constructor.
     */
    IcmpModule() = default;

    /**
     * Copy Constructor.
     */
    IcmpModule(const IcmpModule &other) = delete;

    /**
     * Assignment operator.
     */
    IcmpModule &operator=(const IcmpModule &other) = delete;

    /**
     * Destructor.
     */
    ~IcmpModule() = default;

    void readPacket(Util::Stream::ByteArrayInputStream &stream, LayerInformation information, Device::Network::NetworkDevice &device) override;

    static void writePacket(Util::Network::Icmp::IcmpHeader::Type type, uint8_t code, const Util::Network::Ip4::Ip4Address &sourceAddress,
                            const Util::Network::Ip4::Ip4Address &destinationAddress, const uint8_t *buffer, uint16_t length);

    static void
    sendEchoReply(const Util::Network::Ip4::Ip4Address &sourceAddress, const Util::Network::Ip4::Ip4Address &destinationAddress,
                  const Util::Network::Icmp::EchoHeader &requestHeader, const uint8_t *buffer, uint16_t length,
                  Device::Network::NetworkDevice &device);

private:

    static Kernel::Logger log;
};

}

#endif
