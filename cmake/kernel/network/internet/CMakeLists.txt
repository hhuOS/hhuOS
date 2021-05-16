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
        ${HHUOS_SRC_DIR}/kernel/network/internet/icmp/ICMPEchoRequest.cpp

        ${HHUOS_SRC_DIR}/kernel/network/internet/routing/IP4Route.cpp
        ${HHUOS_SRC_DIR}/kernel/network/internet/routing/IP4RoutingModule.cpp

        ${HHUOS_SRC_DIR}/kernel/network/internet/arp/ARPModule.cpp
        ${HHUOS_SRC_DIR}/kernel/network/internet/arp/ARPRequest.cpp
        ${HHUOS_SRC_DIR}/kernel/network/internet/arp/ARPResponse.cpp

        ${HHUOS_SRC_DIR}/kernel/network/internet/IP4Netmask.cpp
        ${HHUOS_SRC_DIR}/kernel/network/internet/IP4Address.cpp
        ${HHUOS_SRC_DIR}/kernel/network/internet/IP4Datagram.cpp
        ${HHUOS_SRC_DIR}/kernel/network/internet/IP4Module.cpp

)
