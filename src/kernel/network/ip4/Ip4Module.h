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

#ifndef HHUOS_IP4MODULE_H
#define HHUOS_IP4MODULE_H

#include <cstdint>

#include "kernel/network/NetworkModule.h"
#include "lib/util/network/ip4/Ip4Header.h"
#include "Ip4RoutingModule.h"
#include "lib/util/data/Array.h"
#include "lib/util/data/ArrayList.h"
#include "lib/util/data/Collection.h"
#include "lib/util/data/Iterator.h"
#include "lib/util/memory/String.h"

namespace Device {
namespace Network {
class NetworkDevice;
}  // namespace Network
}  // namespace Device
namespace Kernel {
class Logger;
}  // namespace Kernel
namespace Kernel::Network {
namespace Ip4 {
class Ip4Interface;
}  // namespace Ip4
}  // namespace Network
namespace Util {
namespace Network {
namespace Ip4 {
class Ip4Address;
class Ip4NetworkMask;
}  // namespace Ip4
}  // namespace Network

namespace Stream {
class ByteArrayInputStream;
class ByteArrayOutputStream;
}  // namespace Stream
}  // namespace Util

namespace Kernel::Network::Ip4 {

class Ip4Module : public NetworkModule {

public:
    /**
     * Default Constructor.
     */
    Ip4Module() = default;

    /**
     * Copy Constructor.
     */
    Ip4Module(const Ip4Module &other) = delete;

    /**
     * Assignment operator.
     */
    Ip4Module &operator=(const Ip4Module &other) = delete;

    /**
     * Destructor.
     */
    ~Ip4Module() = default;

    Ip4Interface& getInterface(const Util::Memory::String &deviceIdentifier);

    Ip4RoutingModule& getRoutingModule();

    void registerInterface(const Util::Network::Ip4::Ip4Address &address, const Util::Network::Ip4::Ip4Address &networkAddress, const Util::Network::Ip4::Ip4NetworkMask &networkMask, Device::Network::NetworkDevice &device);

    void readPacket(Util::Stream::ByteArrayInputStream &stream, LayerInformation information, Device::Network::NetworkDevice &device) override;

    static const Ip4Interface &
    writeHeader(Util::Stream::ByteArrayOutputStream &stream, const Util::Network::Ip4::Ip4Address &sourceAddress,
                const Util::Network::Ip4::Ip4Address &destinationAddress,
                Util::Network::Ip4::Ip4Header::Protocol protocol, uint16_t payloadLength);

    static uint16_t calculateChecksum(const uint8_t *buffer, uint32_t offset, uint32_t length);

private:

    Ip4RoutingModule routingModule;
    Util::Data::ArrayList<Ip4Interface*> interfaces;

    static Kernel::Logger log;
};

}

#endif
