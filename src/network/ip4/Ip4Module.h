/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Hannes Feil, Michael Schoettner
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

#ifndef HHUOS_IP4MODULE_H
#define HHUOS_IP4MODULE_H

#include "network/NetworkModule.h"
#include "Ip4Header.h"

namespace Network::Ip4 {

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

    void readPacket(Util::Stream::InputStream &stream, Device::Network::NetworkDevice &device) override;

    void writeHeader(Util::Stream::OutputStream &stream, Device::Network::NetworkDevice &device, const Ip4Address &destinationAddress, Ip4Header::Protocol protocol);

private:

    static Kernel::Logger log;
};

}

#endif
