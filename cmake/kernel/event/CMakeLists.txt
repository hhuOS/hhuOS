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
        ${HHUOS_SRC_DIR}/kernel/event/Event.cpp
        ${HHUOS_SRC_DIR}/kernel/event/EventPublisher.cpp

        ${HHUOS_SRC_DIR}/kernel/event/input/KeyEvent.cpp
        ${HHUOS_SRC_DIR}/kernel/event/input/MouseClickedEvent.cpp
        ${HHUOS_SRC_DIR}/kernel/event/input/MouseDoubleClickedEvent.cpp
        ${HHUOS_SRC_DIR}/kernel/event/input/MouseMovedEvent.cpp
        ${HHUOS_SRC_DIR}/kernel/event/input/MouseReleasedEvent.cpp

        ${HHUOS_SRC_DIR}/kernel/event/network/ReceiveEvent.cpp
        ${HHUOS_SRC_DIR}/kernel/event/network/IP4SendEvent.cpp
        ${HHUOS_SRC_DIR}/kernel/event/network/IP4ReceiveEvent.cpp
        ${HHUOS_SRC_DIR}/kernel/event/network/ARPReceiveEvent.cpp
        ${HHUOS_SRC_DIR}/kernel/event/network/EthernetSendEvent.cpp
        ${HHUOS_SRC_DIR}/kernel/event/network/EthernetReceiveEvent.cpp
        ${HHUOS_SRC_DIR}/kernel/event/network/ICMP4SendEvent.cpp
        ${HHUOS_SRC_DIR}/kernel/event/network/ICMP4ReceiveEvent.cpp
        ${HHUOS_SRC_DIR}/kernel/event/network/UDP4SendEvent.cpp
        ${HHUOS_SRC_DIR}/kernel/event/network/UDP4ReceiveEvent.cpp

        ${HHUOS_SRC_DIR}/kernel/event/storage/StorageAddEvent.cpp
        ${HHUOS_SRC_DIR}/kernel/event/storage/StorageRemoveEvent.cpp)
