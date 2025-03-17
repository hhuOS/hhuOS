/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_ETHERNETSOCKET_H
#define HHUOS_ETHERNETSOCKET_H

#include "kernel/network/DatagramSocket.h"

namespace Kernel::Network::Ethernet {

class EthernetSocket : public DatagramSocket {

public:
    /**
     * Default Constructor.
     */
    EthernetSocket();

    /**
     * Copy Constructor.
     */
    EthernetSocket(const EthernetSocket &other) = delete;

    /**
     * Assignment operator.
     */
    EthernetSocket &operator=(const EthernetSocket &other) = delete;

    /**
     * Destructor.
     */
    ~EthernetSocket() override;

    bool send(const Util::Network::Datagram &datagram) override;
};

}

#endif
