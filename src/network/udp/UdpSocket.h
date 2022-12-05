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

#ifndef HHUOS_UDPSOCKET_H
#define HHUOS_UDPSOCKET_H

#include "UdpDatagram.h"
#include "lib/util/data/ArrayListBlockingQueue.h"
#include "lib/util/async/Spinlock.h"

namespace Network::Udp {

class UdpSocket {

friend class UdpModule;

public:
    /**
     * Constructor.
     */
    explicit UdpSocket(uint16_t port);

    /**
     * Copy Constructor.
     */
    UdpSocket(const UdpSocket &other) = delete;

    /**
     * Assignment operator.
     */
    UdpSocket &operator=(const UdpSocket &other) = delete;

    /**
     * Destructor.
     */
    ~UdpSocket() = default;

    void send(const UdpDatagram &datagram) const;

    UdpDatagram receive();

    [[nodiscard]] uint16_t getPort() const;

private:

    void handleIncomingDatagram(const UdpDatagram &datagram);

    uint16_t port;
    Util::Async::Spinlock lock;
    Util::Data::ArrayListBlockingQueue<UdpDatagram> incomingDatagramQueue;
};

}

#endif
