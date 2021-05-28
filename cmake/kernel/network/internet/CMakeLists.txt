# Copyright (C) 2019 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
# Heinrich-Heine University
#
# This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>

cmake_minimum_required(VERSION 3.7)

target_sources(kernel PUBLIC
        ${HHUOS_SRC_DIR}/kernel/network/internet/icmp/ICMP4Module.cpp
        ${HHUOS_SRC_DIR}/kernel/network/internet/icmp/messages/ICMP4EchoReply.cpp
        ${HHUOS_SRC_DIR}/kernel/network/internet/icmp/messages/ICMP4DestinationUnreachable.cpp
        ${HHUOS_SRC_DIR}/kernel/network/internet/icmp/messages/ICMP4Echo.cpp
        ${HHUOS_SRC_DIR}/kernel/network/internet/icmp/messages/ICMP4TimeExceeded.cpp

        ${HHUOS_SRC_DIR}/kernel/network/internet/routing/IP4RoutingModule.cpp
        ${HHUOS_SRC_DIR}/kernel/network/internet/routing/IP4Route.cpp

        ${HHUOS_SRC_DIR}/kernel/network/internet/arp/ARPModule.cpp
        ${HHUOS_SRC_DIR}/kernel/network/internet/arp/ARPEntry.cpp
        ${HHUOS_SRC_DIR}/kernel/network/internet/arp/ARPRequest.cpp
        ${HHUOS_SRC_DIR}/kernel/network/internet/arp/ARPResponse.cpp

        ${HHUOS_SRC_DIR}/kernel/network/internet/addressing/IP4Address.cpp
        ${HHUOS_SRC_DIR}/kernel/network/internet/addressing/IP4Netmask.cpp

        ${HHUOS_SRC_DIR}/kernel/network/internet/IP4Interface.cpp
        ${HHUOS_SRC_DIR}/kernel/network/internet/IP4Module.cpp
        ${HHUOS_SRC_DIR}/kernel/network/internet/IP4HeaderChecksum.cpp
        ${HHUOS_SRC_DIR}/kernel/network/internet/IP4Datagram.cpp

)
