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

#ifndef HHUOS_ICMPMODULE_H
#define HHUOS_ICMPMODULE_H

#include "network/NetworkModule.h"
#include "EchoMessage.h"
#include "network/ip4/Ip4Address.h"

namespace Network::Icmp {

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

    static void sendEchoReply(const Network::Ip4::Ip4Address &destinationAddress, const EchoMessage &request, Device::Network::NetworkDevice &device);

private:

    static Kernel::Logger log;
};

}

#endif
