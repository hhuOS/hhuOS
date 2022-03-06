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

project(tree)
message(STATUS "Project " ${PROJECT_NAME})

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/hhuOS.initrd"
        COMMAND mkdir -p "${HHUOS_ROOT_DIR}/initrd/bin"
        COMMAND cp "$<TARGET_FILE:cat>" "${HHUOS_ROOT_DIR}/initrd/bin/cat"
        COMMAND cp "$<TARGET_FILE:color>" "${HHUOS_ROOT_DIR}/initrd/bin/color"
        COMMAND cp "$<TARGET_FILE:date>" "${HHUOS_ROOT_DIR}/initrd/bin/date"
        COMMAND cp "$<TARGET_FILE:echo>" "${HHUOS_ROOT_DIR}/initrd/bin/echo"
        COMMAND cp "$<TARGET_FILE:hello>" "${HHUOS_ROOT_DIR}/initrd/bin/hello"
        COMMAND cp "$<TARGET_FILE:ls>" "${HHUOS_ROOT_DIR}/initrd/bin/ls"
        COMMAND cp "$<TARGET_FILE:tree>" "${HHUOS_ROOT_DIR}/initrd/bin/tree"
        COMMAND cp "$<TARGET_FILE:uptime>" "${HHUOS_ROOT_DIR}/initrd/bin/uptime"
        COMMAND tar -C "${HHUOS_ROOT_DIR}/initrd/" --xform s:'./':: -cf "${CMAKE_BINARY_DIR}/hhuOS.initrd" ./
        COMMAND rm -f "${HHUOS_ROOT_DIR}/hhuOS.img" "${HHUOS_ROOT_DIR}/hhuOS.iso"
        DEPENDS cat date color echo hello ls tree uptime)

add_custom_target(initrd DEPENDS cat color date echo hello ls tree uptime "${CMAKE_BINARY_DIR}/hhuOS.initrd")