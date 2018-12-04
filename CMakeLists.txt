# Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
PROJECT(hhuOS)

INCLUDE_DIRECTORIES(./src/os)

SET(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_FLAGS -m32)
set(CMAKE_CXX_FLAGS -m32)

FILE(GLOB_RECURSE SOURCE_FILES "src/os/*.c" "src/os/*.cpp" "src/os/*.h" "src/os/*.asm" "src/modules/*.c" "src/modules/*.cpp" "src/modules/*.h" "src/modules/*.asm" "Makefile")

ADD_EXECUTABLE(hhuOS ${SOURCE_FILES} src/os/lib/Prototype.cpp src/os/lib/Prototype.h src/modules/static-heap/StaticHeapModule.cpp src/modules/static-heap/StaticHeapModule.h src/os/apps/MemoryTests/MemoryManagerTest.cpp)
