# Copyright (C) 2018-2022 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

project(application)
message(STATUS "Project " ${PROJECT_NAME})

include_directories(${HHUOS_SRC_DIR})

set(CMAKE_C_FLAGS "-m32 -march=i386 -mfpmath=387 -mno-mmx -mno-sse -mno-avx -Wall -fno-stack-protector -nostdlib -fno-pic -no-pie -ffreestanding")
if(CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 9)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mmanual-endbr")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wl,--build-id=none -Wno-non-virtual-dtor -fno-threadsafe-statics -fno-use-cxa-atexit -fno-rtti -fno-exceptions -std=c++17 -T ${HHUOS_SRC_DIR}/application/link.ld")

# Add subdirectories
add_subdirectory(shell)
add_subdirectory(cat)
add_subdirectory(color)
add_subdirectory(cp)
add_subdirectory(date)
add_subdirectory(echo)
add_subdirectory(fault)
add_subdirectory(hello)
add_subdirectory(ls)
add_subdirectory(mkdir)
add_subdirectory(pwd)
add_subdirectory(rm)
add_subdirectory(rmdir)
add_subdirectory(touch)
add_subdirectory(tree)
add_subdirectory(uptime)
