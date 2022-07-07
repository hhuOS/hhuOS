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

project(hdd0)
message(STATUS "Project " ${PROJECT_NAME})

add_custom_command(OUTPUT "${HHUOS_ROOT_DIR}/hdd0.img"
        COMMAND /bin/mkdir -p "${HHUOS_ROOT_DIR}/hdd0/img/bin"
        COMMAND /bin/cp "$<TARGET_FILE:shell>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/shell"
        COMMAND /bin/cp "$<TARGET_FILE:ant>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/ant"
        COMMAND /bin/cp "$<TARGET_FILE:cat>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/cat"
        COMMAND /bin/cp "$<TARGET_FILE:color>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/color"
        COMMAND /bin/cp "$<TARGET_FILE:cp>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/cp"
        COMMAND /bin/cp "$<TARGET_FILE:cube>" "${HHUOS_ROOT_DIR}/initrd/bin/cube"
        COMMAND /bin/cp "$<TARGET_FILE:date>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/date"
        COMMAND /bin/cp "$<TARGET_FILE:echo>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/echo"
        COMMAND /bin/cp "$<TARGET_FILE:fault>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/fault"
        COMMAND /bin/cp "$<TARGET_FILE:hello>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/hello"
        COMMAND /bin/cp "$<TARGET_FILE:ls>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/ls"
        COMMAND /bin/cp "$<TARGET_FILE:membench>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/membench"
        COMMAND /bin/cp "$<TARGET_FILE:mkdir>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/mkdir"
        COMMAND /bin/cp "$<TARGET_FILE:pwd>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/pwd"
        COMMAND /bin/cp "$<TARGET_FILE:rm>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/rm"
        COMMAND /bin/cp "$<TARGET_FILE:rmdir>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/rmdir"
        COMMAND /bin/cp "$<TARGET_FILE:shutdown>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/shutdown"
        COMMAND /bin/cp "$<TARGET_FILE:touch>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/touch"
        COMMAND /bin/cp "$<TARGET_FILE:tree>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/tree"
        COMMAND /bin/cp "$<TARGET_FILE:uptime>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/uptime"
        WORKING_DIRECTORY ${HHUOS_ROOT_DIR}/hdd0 COMMAND ${HHUOS_ROOT_DIR}/hdd0/build.sh
        DEPENDS shell ant cat color cp cube date echo fault hello ls mkdir membench pwd rm rmdir shutdown touch tree uptime)

add_custom_target(${PROJECT_NAME} DEPENDS shell ant cat color cp cube date echo fault hello ls mkdir membench pwd rm rmdir shutdown touch tree uptime "${HHUOS_ROOT_DIR}/hdd0.img")