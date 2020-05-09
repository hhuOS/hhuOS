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
        ${HHUOS_SRC_DIR}/device/network/e1000/driver/intel82541IP/Intel82541IP.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/driver/intel82541IP/BufferedInterruptCause82541IP.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/driver/intel82541IP/BufferedReceiveErrors82541IP.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/driver/intel82541IP/Eeprom82541IP.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/driver/intel82541IP/InterruptDisable82541IP.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/driver/intel82541IP/InterruptEnable82541IP.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/driver/intel82541IP/MainControl82541IP.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/driver/intel82541IP/ReceiveControl82541IP.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/driver/intel82541IP/TransmitControl82541IP.cpp
        ${HHUOS_SRC_DIR}/device/network/e1000/driver/intel82541IP/TransmitDescriptor82541IP.cpp)
