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

target_sources(kernel PUBLIC
        ${HHUOS_SRC_DIR}/kernel/memory/BitmapMemoryManager.cpp
        ${HHUOS_SRC_DIR}/kernel/memory/GlobalDescriptorTable.cpp
        ${HHUOS_SRC_DIR}/kernel/memory/MemoryStatusNode.cpp
        ${HHUOS_SRC_DIR}/kernel/memory/PageFrameAllocator.cpp
        ${HHUOS_SRC_DIR}/kernel/memory/Paging.cpp
        ${HHUOS_SRC_DIR}/kernel/memory/PagingAreaManager.cpp
        ${HHUOS_SRC_DIR}/kernel/memory/PagingAreaManagerRefillRunnable.cpp
        ${HHUOS_SRC_DIR}/kernel/memory/SlabAllocator.cpp
        ${HHUOS_SRC_DIR}/kernel/memory/TableMemoryManager.cpp
        ${HHUOS_SRC_DIR}/kernel/memory/VirtualAddressSpace.cpp)
