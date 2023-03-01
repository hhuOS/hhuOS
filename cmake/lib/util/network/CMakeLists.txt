# Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
# Institute of Computer Science, Department Operating Systems
# Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
#
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

cmake_minimum_required(VERSION 3.14)

project(lib.network)
message(STATUS "Project " ${PROJECT_NAME})

include_directories(${HHUOS_SRC_DIR})

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

# Add subdirectories
target_sources(${PROJECT_NAME} PUBLIC
        ${HHUOS_SRC_DIR}/lib/util/network/Datagram.cpp
        ${HHUOS_SRC_DIR}/lib/util/network/MacAddress.cpp
        ${HHUOS_SRC_DIR}/lib/util/network/NetworkAddress.cpp
        ${HHUOS_SRC_DIR}/lib/util/network/NetworkMask.cpp
        ${HHUOS_SRC_DIR}/lib/util/network/NumberUtil.cpp
        ${HHUOS_SRC_DIR}/lib/util/network/Socket.cpp)

# Add subdirectories
add_subdirectory(ethernet)
add_subdirectory(icmp)
add_subdirectory(ip4)
add_subdirectory(udp)

# Kernel space version
project(lib.kernel.network)
message(STATUS "Project " ${PROJECT_NAME})
include_directories(${HHUOS_SRC_DIR})
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib.network)
target_sources(${PROJECT_NAME} PUBLIC ${HHUOS_SRC_DIR}/lib/kernel.cpp)

# User space version
project(lib.user.network)
message(STATUS "Project " ${PROJECT_NAME})
include_directories(${HHUOS_SRC_DIR})
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib.network)
target_sources(${PROJECT_NAME} PUBLIC ${HHUOS_SRC_DIR}/lib/user.cpp)