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

target_sources(filesystem PUBLIC
        ${HHUOS_SRC_DIR}/filesystem/memory/BufferNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/memory/MemoryDirectoryNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/memory/MemoryDriver.cpp
        ${HHUOS_SRC_DIR}/filesystem/memory/MemoryFileNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/memory/MemoryNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/memory/MemoryWrapperNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/memory/MountsNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/memory/NullNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/memory/RandomNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/memory/StreamNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/memory/StringNode.cpp
        ${HHUOS_SRC_DIR}/filesystem/memory/ZeroNode.cpp)
