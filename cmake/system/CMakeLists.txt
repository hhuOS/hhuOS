# Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

cmake_minimum_required(VERSION 3.7)

project(system)
message(STATUS "Project " ${PROJECT_NAME})

include_directories(${HHUOS_SRC_DIR})

# Set source files
set(SOURCE_FILES
        ${HHUOS_SRC_DIR}/boot.asm
        ${HHUOS_SRC_DIR}/BuildConfig.cpp
        ${HHUOS_SRC_DIR}/GatesOfHell.cpp
        ${HHUOS_SRC_DIR}/SchedulerSign.cpp
        ${HHUOS_SRC_DIR}/asm_interface.cpp
        ${HHUOS_SRC_DIR}/constants.asm ../../src/device/power/Machine.h ../../src/device/power/default/DefaultMachine.cpp ../../src/device/power/default/DefaultMachine.h ../../src/kernel/service/PowerManagementService.cpp ../../src/kernel/service/PowerManagementService.h ../../src/lib/util/system/Machine.cpp ../../src/lib/util/system/Machine.h)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} device filesystem kernel lib.kernel)