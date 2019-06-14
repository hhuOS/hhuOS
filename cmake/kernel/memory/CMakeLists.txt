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

cmake_minimum_required(VERSION 3.5)

target_sources(kernel PUBLIC
        ${HHUOS_SRC_DIR}/kernel/memory/paging.asm
        ${HHUOS_SRC_DIR}/kernel/memory/Paging.cpp
        ${HHUOS_SRC_DIR}/kernel/memory/PageDirectory.cpp
        ${HHUOS_SRC_DIR}/kernel/memory/VirtualAddressSpace.cpp
        ${HHUOS_SRC_DIR}/kernel/memory/manager/MemoryManager.cpp
        ${HHUOS_SRC_DIR}/kernel/memory/manager/StaticHeapMemoryManager.cpp
        ${HHUOS_SRC_DIR}/kernel/memory/manager/BitmapMemoryManager.cpp
        ${HHUOS_SRC_DIR}/kernel/memory/manager/FreeListMemoryManager.cpp
        ${HHUOS_SRC_DIR}/kernel/memory/manager/IOMemoryManager.cpp
        ${HHUOS_SRC_DIR}/kernel/memory/manager/IsaDmaMemoryManager.cpp
        ${HHUOS_SRC_DIR}/kernel/memory/manager/PageFrameAllocator.cpp
        ${HHUOS_SRC_DIR}/kernel/memory/manager/PagingAreaManager.cpp
        ${HHUOS_SRC_DIR}/kernel/memory/manager/special/AtomicBitmapMemoryManager.cpp)