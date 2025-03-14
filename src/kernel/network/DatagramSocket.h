/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_DATAGRAMSOCKET_H
#define HHUOS_DATAGRAMSOCKET_H

#include <stdint.h>

#include "Socket.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/collection/ArrayListBlockingQueue.h"
#include "lib/util/io/file/File.h"
#include "lib/util/network/Socket.h"

namespace Util {
namespace Network {
class Datagram;
}  // namespace Network
}  // namespace Util

namespace Kernel {
namespace Network {
class NetworkModule;
}  // namespace Network
}  // namespace Kernel

namespace Kernel::Network {

namespace Udp {
class UdpModule;
}

namespace Ip4 {
class Ip4Module;
}

namespace Icmp {
class IcmpModule;
}

namespace Ethernet {
class EthernetModule;
}

class DatagramSocket : public Socket {

friend class Udp::UdpModule;
friend class Ip4::Ip4Module;
friend class Icmp::IcmpModule;
friend class Ethernet::EthernetModule;

public:
    /**
     * Constructor.
     */
    explicit DatagramSocket(NetworkModule &networkModule, Util::Network::Socket::Type type);

    /**
     * Copy Constructor.
     */
    DatagramSocket(const DatagramSocket &other) = delete;

    /**
     * Assignment operator.
     */
    DatagramSocket &operator=(const DatagramSocket &other) = delete;

    /**
     * Destructor.
     */
    ~DatagramSocket() override = default;

    Util::Network::Datagram* receive() override;

    /**
     * Overriding function from Node.
     */
    Util::String getName() override;

    /**
     * Overriding function from Node.
     */
    Util::Io::File::Type getType() override;

    /**
     * Overriding function from Node.
     */
    uint64_t getLength() override;

    /**
     * Overriding function from Node.
     */
    Util::Array<Util::String> getChildren() override;

    /**
     * Overriding function from Node.
     */
    uint64_t readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from Node.
     */
    uint64_t writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from Node.
     */
    bool isReadyToRead() override;

private:

    void handleIncomingDatagram(Util::Network::Datagram *datagram);

    Util::Async::Spinlock lock;
    Util::ArrayListBlockingQueue<Util::Network::Datagram*> incomingDatagramQueue;
};

}

#endif
