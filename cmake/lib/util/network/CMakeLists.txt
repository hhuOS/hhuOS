# Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
# Institute of Computer Science, Department Operating Systems
# Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
# Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
# This project has been supported by several students.
# A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

project(lib.network)
message(STATUS "Project " ${PROJECT_NAME})

include_directories(${HHUOS_SRC_DIR} ${HHUOS_SRC_DIR}/lib ${HHUOS_SRC_DIR}/lib/libc)
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_sources(${PROJECT_NAME} PUBLIC
        ${HHUOS_SRC_DIR}/lib/util/network/Datagram.cpp
        ${HHUOS_SRC_DIR}/lib/util/network/MacAddress.cpp
        ${HHUOS_SRC_DIR}/lib/util/network/Socket.cpp
        ${HHUOS_SRC_DIR}/lib/util/network/ethernet/EthernetHeader.cpp
        ${HHUOS_SRC_DIR}/lib/util/network/icmp/IcmpHeader.cpp
        ${HHUOS_SRC_DIR}/lib/util/network/icmp/EchoHeader.cpp
        ${HHUOS_SRC_DIR}/lib/util/network/ip4/Ip4Address.cpp
        ${HHUOS_SRC_DIR}/lib/util/network/ip4/Ip4Header.cpp
        ${HHUOS_SRC_DIR}/lib/util/network/ip4/Ip4SubnetAddress.cpp
        ${HHUOS_SRC_DIR}/lib/util/network/udp/UdpHeader.cpp)

# Kernel space version
project(lib.kernel.network)
message(STATUS "Project " ${PROJECT_NAME})
include_directories(${HHUOS_SRC_DIR} ${HHUOS_SRC_DIR}/lib ${HHUOS_SRC_DIR}/lib/libc)
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib.network)
target_sources(${PROJECT_NAME} PUBLIC ${HHUOS_SRC_DIR}/lib/kernel.cpp)

# User space version
project(lib.user.network)
message(STATUS "Project " ${PROJECT_NAME})
include_directories(${HHUOS_SRC_DIR} ${HHUOS_SRC_DIR}/lib ${HHUOS_SRC_DIR}/lib/libc)
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib.network)
target_sources(${PROJECT_NAME} PUBLIC ${HHUOS_SRC_DIR}/lib/user.cpp)