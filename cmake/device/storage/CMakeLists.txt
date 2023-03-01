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
 
target_sources(device PUBLIC
        ${HHUOS_SRC_DIR}/device/storage/ChsConverter.cpp
        ${HHUOS_SRC_DIR}/device/storage/Partition.cpp
        ${HHUOS_SRC_DIR}/device/storage/PartitionHandler.cpp
        ${HHUOS_SRC_DIR}/device/storage/StorageDevice.cpp
        ${HHUOS_SRC_DIR}/device/storage/floppy/FloppyController.cpp
        ${HHUOS_SRC_DIR}/device/storage/floppy/FloppyDevice.cpp
        ${HHUOS_SRC_DIR}/device/storage/floppy/FloppyMotorControlRunnable.cpp
        ${HHUOS_SRC_DIR}/device/storage/ide/IdeController.cpp
        ${HHUOS_SRC_DIR}/device/storage/ide/IdeDevice.cpp
        ${HHUOS_SRC_DIR}/device/storage/virtual/VirtualDiskDrive.cpp)
