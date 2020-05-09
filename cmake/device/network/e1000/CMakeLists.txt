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
 
target_sources(device PUBLIC
        ${HHUOS_SRC_DIR}/device/network/e1000/E1000.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/McBuffered.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/descriptorRing/HrDefault.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/descriptorRing/HrReceive.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/descriptorRing/HrTransmit.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/eeprom/ErDefault.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/general/DcDefault.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/general/RgSimple.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/interrupt/IcBufferedDefault.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/interrupt/IdDefault.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/interrupt/IeDefault.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/nodes/MacAddressNode.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/receive/RcDefault.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/receive/ReceiveRingSimple.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/receive/descriptor/RdDefault.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/receive/descriptor/ReceiveDescriptorSimple.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/receive/descriptor/field/ReBufferedDefault.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/receive/descriptor/field/RsBufferedSimple.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/transmit/TcDefault.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/transmit/TransmitRingSimple.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/transmit/descriptor/legacy/TdDefault.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/transmit/descriptor/legacy/field/LcDefault.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/transmit/descriptor/legacy/field/LegacyCommandSimple.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/transmit/descriptor/legacy/field/TransmitStatusSimple.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/transmit/descriptor/legacy/field/TsDefault.cpp)

add_subdirectory(intel82540em)
add_subdirectory(intel82541ip)
