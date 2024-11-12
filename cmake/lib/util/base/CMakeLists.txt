# Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

project(lib.base)
message(STATUS "Project " ${PROJECT_NAME})

include_directories(${HHUOS_SRC_DIR})

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} lib.async lib.hardware lib.io lib.reflection)

target_sources(${PROJECT_NAME} PUBLIC
        ${HHUOS_SRC_DIR}/lib/util/base/operators.cpp
        ${HHUOS_SRC_DIR}/lib/util/base/Address.cpp
        ${HHUOS_SRC_DIR}/lib/util/base/ArgumentParser.cpp
		${HHUOS_SRC_DIR}/lib/util/base/CharacterTypes.cpp
        ${HHUOS_SRC_DIR}/lib/util/base/Exception.cpp
        ${HHUOS_SRC_DIR}/lib/util/base/FreeListMemoryManager.cpp
        ${HHUOS_SRC_DIR}/lib/util/base/String.cpp
        ${HHUOS_SRC_DIR}/lib/util/base/System.cpp
		${HHUOS_SRC_DIR}/lib/util/base/WideChar.cpp)

# Kernel space version
project(lib.kernel.base)
message(STATUS "Project " ${PROJECT_NAME})
include_directories(${HHUOS_SRC_DIR})
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib.base)
target_sources(${PROJECT_NAME} PUBLIC ${HHUOS_SRC_DIR}/lib/kernel.cpp)

# User space version
project(lib.user.base)
message(STATUS "Project " ${PROJECT_NAME})
include_directories(${HHUOS_SRC_DIR})
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib.base)
target_sources(${PROJECT_NAME} PUBLIC ${HHUOS_SRC_DIR}/lib/user.cpp)