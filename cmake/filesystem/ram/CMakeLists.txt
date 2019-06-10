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
 
target_sources(filesystem PUBLIC
        ${HHUOS_SRC_DIR}/filesystem/ram/RamFsDriver.cpp
        ${HHUOS_SRC_DIR}/filesystem/ram/RamFsNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/ram/VirtualNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/ram/nodes/PciNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/ram/nodes/StderrNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/ram/nodes/StdoutNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/ram/nodes/StorageNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/ram/nodes/memory/MemoryNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/ram/nodes/memory/IoMemoryNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/ram/nodes/memory/KernelHeapNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/ram/nodes/memory/PagingAreaNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/ram/nodes/memory/PhysicalMemoryNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/ram/nodes/util/RandomNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/ram/nodes/util/ZeroNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/ram/nodes/video/GraphicsNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/ram/nodes/video/CurrentGraphicsDriverNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/ram/nodes/video/CurrentResolutionNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/ram/nodes/video/GraphicsDeviceNameNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/ram/nodes/video/GraphicsDriversNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/ram/nodes/video/GraphicsMemoryNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/ram/nodes/video/GraphicsResolutionsNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/ram/nodes/video/GraphicsVendorNameNode.cpp)
