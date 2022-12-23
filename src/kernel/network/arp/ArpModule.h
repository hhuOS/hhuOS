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

#ifndef HHUOS_ARPMODULE_H
#define HHUOS_ARPMODULE_H

#include <cstdint>

#include "lib/util/async/ReentrantSpinlock.h"
#include "kernel/network/NetworkModule.h"
#include "ArpHeader.h"
#include "ArpEntry.h"
#include "lib/util/data/Array.h"
#include "lib/util/data/ArrayList.h"
#include "lib/util/data/Collection.h"
#include "lib/util/data/Iterator.h"
#include "lib/util/network/MacAddress.h"

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
namespace Ip4 {
class Ip4Address;
}  // namespace Ip4
}  // namespace Network

namespace Stream {
class ByteArrayInputStream;
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Kernel::Network::Arp {

class ArpModule : public NetworkModule {

public:
    /**
     * Default Constructor.
     */
    ArpModule() = default;

    /**
     * Copy Constructor.
     */
    ArpModule(const ArpModule &other) = delete;

    /**
     * Assignment operator.
     */
    ArpModule &operator=(const ArpModule &other) = delete;

    /**
     * Destructor.
     */
    ~ArpModule() = default;

    void readPacket(Util::Stream::ByteArrayInputStream &stream, LayerInformation information, Device::Network::NetworkDevice &device) override;

    bool resolveAddress(const Util::Network::Ip4::Ip4Address &protocolAddress, Util::Network::MacAddress &hardwareAddress, Device::Network::NetworkDevice &device);

    static void writeHeader(Util::Stream::OutputStream &stream, ArpHeader::Operation operation, Device::Network::NetworkDevice &device, const Util::Network::MacAddress &destinationAddress);

    void setEntry(const Util::Network::Ip4::Ip4Address &protocolAddress, const Util::Network::MacAddress &hardwareAddress);

private:

    Util::Network::MacAddress getHardwareAddress(const Util::Network::Ip4::Ip4Address &protocolAddress);

    bool hasHardwareAddress(const Util::Network::Ip4::Ip4Address &protocolAddress);

    void handleRequest(const Util::Network::MacAddress &sourceHardwareAddress, const Util::Network::Ip4::Ip4Address &sourceAddress, const Util::Network::Ip4::Ip4Address &targetProtocolAddress, Device::Network::NetworkDevice &device);

    void handleReply(const Util::Network::MacAddress &sourceHardwareAddress, const Util::Network::Ip4::Ip4Address &sourceAddress, const Util::Network::MacAddress &targetHardwareAddress, const Util::Network::Ip4::Ip4Address &targetProtocolAddress);

    Util::Async::ReentrantSpinlock lock;
    Util::Data::ArrayList<ArpEntry> arpCache;

    static Kernel::Logger log;

    static const constexpr uint32_t REQUEST_WAIT_TIME = 100;
    static const constexpr uint32_t MAX_REQUEST_RETRIES = 10;
};

}

#endif
