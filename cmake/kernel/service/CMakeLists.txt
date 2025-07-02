# Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

target_sources(kernel PUBLIC
        ${HHUOS_SRC_DIR}/kernel/service/Service.cpp
        ${HHUOS_SRC_DIR}/kernel/service/CpuService.cpp
        ${HHUOS_SRC_DIR}/kernel/service/FilesystemService.cpp
        ${HHUOS_SRC_DIR}/kernel/service/InformationService.cpp
        ${HHUOS_SRC_DIR}/kernel/service/InterruptService.cpp
        ${HHUOS_SRC_DIR}/kernel/service/MemoryService.cpp
        ${HHUOS_SRC_DIR}/kernel/service/NetworkService.cpp
        ${HHUOS_SRC_DIR}/kernel/service/PowerManagementService.cpp
        ${HHUOS_SRC_DIR}/kernel/service/ProcessService.cpp
        ${HHUOS_SRC_DIR}/kernel/service/SoundService.cpp
        ${HHUOS_SRC_DIR}/kernel/service/StorageService.cpp
        ${HHUOS_SRC_DIR}/kernel/service/TimeService.cpp)
