# Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

project(towboot)
message(STATUS "Project " ${PROJECT_NAME})

add_custom_command(OUTPUT "${HHUOS_ROOT_DIR}/hhuOS.img"
        WORKING_DIRECTORY "${HHUOS_ROOT_DIR}/loader/towboot/"
        COMMAND cp "$<TARGET_FILE:system>" "${HHUOS_ROOT_DIR}/loader/towboot/hhuOS.bin"
        COMMAND cp "${CMAKE_BINARY_DIR}/hhuOS.initrd" "${HHUOS_ROOT_DIR}/loader/towboot/hhuOS.initrd"
        COMMAND "${HHUOS_ROOT_DIR}/loader/towboot/build.sh"
        COMMAND mv "${HHUOS_ROOT_DIR}/loader/towboot/hhuOS.img" "${HHUOS_ROOT_DIR}"
        DEPENDS system)

add_custom_target(towboot DEPENDS system initrd "${HHUOS_ROOT_DIR}/hhuOS.img")