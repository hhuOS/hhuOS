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

#include "NetworkStack.h"

#include "lib/util/network/ethernet/EthernetHeader.h"
#include "lib/util/network/ip4/Ip4Header.h"

namespace Kernel::Network {

NetworkStack::NetworkStack() {
    ethernetModule.registerNextLayerModule(Util::Network::Ethernet::EthernetHeader::ARP, arpModule);
    ethernetModule.registerNextLayerModule(Util::Network::Ethernet::EthernetHeader::IP4, ip4Module);
    ip4Module.registerNextLayerModule(Util::Network::Ip4::Ip4Header::ICMP, icmpModule);
    ip4Module.registerNextLayerModule(Util::Network::Ip4::Ip4Header::UDP, udpModule);
}

Network::Ethernet::EthernetModule &NetworkStack::getEthernetModule() {
    return ethernetModule;
}

Network::Arp::ArpModule &NetworkStack::getArpModule() {
    return arpModule;
}

Network::Ip4::Ip4Module &NetworkStack::getIp4Module() {
    return ip4Module;
}

Icmp::IcmpModule &NetworkStack::getIcmpModule() {
    return icmpModule;
}

Udp::UdpModule &NetworkStack::getUdpModule() {
    return udpModule;
}

}