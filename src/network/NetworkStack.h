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

#ifndef HHUOS_NETWORKSTACK_H
#define HHUOS_NETWORKSTACK_H

#include "network/ip4/Ip4Module.h"
#include "network/arp/ArpModule.h"
#include "network/ethernet/EthernetModule.h"
#include "network/icmp/IcmpModule.h"
#include "network/udp/UdpModule.h"

namespace Network {

class NetworkStack {

public:
    /**
     * Default Constructor.
     */
    NetworkStack();

    /**
     * Copy Constructor.
     */
    NetworkStack(const NetworkStack &other) = delete;

    /**
     * Assignment operator.
     */
    NetworkStack &operator=(const NetworkStack &other) = delete;

    /**
     * Destructor.
     */
    ~NetworkStack() = default;

    Ethernet::EthernetModule& getEthernetModule();

    Arp::ArpModule& getArpModule();

    Ip4::Ip4Module& getIp4Module();

    Icmp::IcmpModule& getIcmpModule();

    Udp::UdpModule& getUdpModule();

private:

    Ethernet::EthernetModule ethernetModule;
    Arp::ArpModule arpModule;
    Ip4::Ip4Module ip4Module;
    Icmp::IcmpModule icmpModule;
    Udp::UdpModule udpModule;
};

}

#endif
