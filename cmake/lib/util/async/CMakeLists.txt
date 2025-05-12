# Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

project(lib.async)
message(STATUS "Project " ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 90)

include_directories(${HHUOS_SRC_DIR} ${HHUOS_SRC_DIR}/lib/libc ${HHUOS_SRC_DIR}/lib/util)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_sources(${PROJECT_NAME} PUBLIC
        ${HHUOS_SRC_DIR}/lib/util/async/Atomic.cpp
        ${HHUOS_SRC_DIR}/lib/util/async/AtomicBitmap.cpp
        ${HHUOS_SRC_DIR}/lib/util/async/BasicRunnable.cpp
        ${HHUOS_SRC_DIR}/lib/util/async/IdGenerator.cpp
        ${HHUOS_SRC_DIR}/lib/util/async/Process.cpp
        ${HHUOS_SRC_DIR}/lib/util/async/ReentrantSpinlock.cpp
        ${HHUOS_SRC_DIR}/lib/util/async/Spinlock.cpp
        ${HHUOS_SRC_DIR}/lib/util/async/Thread.cpp)

# Kernel space version
project(lib.kernel.async)
message(STATUS "Project " ${PROJECT_NAME})
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib.async)
target_sources(${PROJECT_NAME} PUBLIC ${HHUOS_SRC_DIR}/lib/kernel.cpp)

# User space version
project(lib.user.async)
message(STATUS "Project " ${PROJECT_NAME})
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib.async)
target_sources(${PROJECT_NAME} PUBLIC ${HHUOS_SRC_DIR}/lib/user.cpp)