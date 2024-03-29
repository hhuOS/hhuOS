# Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

project(hdd0)
message(STATUS "Project " ${PROJECT_NAME})

add_custom_command(OUTPUT "${HHUOS_ROOT_DIR}/hdd0.img"
        COMMAND /bin/mkdir -p "${HHUOS_ROOT_DIR}/hdd0/img/bin"
        COMMAND /bin/mkdir -p "${HHUOS_ROOT_DIR}/hdd0/img/user"
        COMMAND /bin/mkdir -p "${HHUOS_ROOT_DIR}/hdd0/img/media/floppy"
        COMMAND /bin/cp "$<TARGET_FILE:shell>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/shell"
        COMMAND /bin/cp "$<TARGET_FILE:asciimate>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/asciimate"
        COMMAND /bin/cp "$<TARGET_FILE:battlespace>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/battlespace"
        COMMAND /bin/cp "$<TARGET_FILE:beep>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/beep"
        COMMAND /bin/cp "$<TARGET_FILE:bug>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/bug"
        COMMAND /bin/cp "$<TARGET_FILE:cat>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/cat"
        COMMAND /bin/cp "$<TARGET_FILE:color>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/color"
        COMMAND /bin/cp "$<TARGET_FILE:cp>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/cp"
        COMMAND /bin/cp "$<TARGET_FILE:date>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/date"
        COMMAND /bin/cp "$<TARGET_FILE:demo>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/demo"
        COMMAND /bin/cp "$<TARGET_FILE:dino>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/dino"
        COMMAND /bin/cp "$<TARGET_FILE:echo>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/echo"
        COMMAND /bin/cp "$<TARGET_FILE:head>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/head"
        COMMAND /bin/cp "$<TARGET_FILE:hexdump>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/hexdump"
        COMMAND /bin/cp "$<TARGET_FILE:ip>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/ip"
        COMMAND /bin/cp "$<TARGET_FILE:kill>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/kill"
        COMMAND /bin/cp "$<TARGET_FILE:ls>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/ls"
        COMMAND /bin/cp "$<TARGET_FILE:lvgl_demo>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/lvgl"
        COMMAND /bin/cp "$<TARGET_FILE:membench>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/membench"
        COMMAND /bin/cp "$<TARGET_FILE:mkdir>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/mkdir"
        COMMAND /bin/cp "$<TARGET_FILE:mount>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/mount"
        COMMAND /bin/cp "$<TARGET_FILE:mouse>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/mouse"
        COMMAND /bin/cp "$<TARGET_FILE:ping>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/ping"
        COMMAND /bin/cp "$<TARGET_FILE:play>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/play"
        COMMAND /bin/cp "$<TARGET_FILE:ps>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/ps"
        COMMAND /bin/cp "$<TARGET_FILE:pwd>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/pwd"
        COMMAND /bin/cp "$<TARGET_FILE:rm>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/rm"
        COMMAND /bin/cp "$<TARGET_FILE:rmdir>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/rmdir"
        COMMAND /bin/cp "$<TARGET_FILE:shutdown>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/shutdown"
        COMMAND /bin/cp "$<TARGET_FILE:smbios>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/smbios"
        COMMAND /bin/cp "$<TARGET_FILE:touch>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/touch"
        COMMAND /bin/cp "$<TARGET_FILE:tree>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/tree"
        COMMAND /bin/cp "$<TARGET_FILE:uecho>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/uecho"
        COMMAND /bin/cp "$<TARGET_FILE:unmount>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/unmount"
        COMMAND /bin/cp "$<TARGET_FILE:uptime>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/uptime"
        COMMAND /bin/cp "$<TARGET_FILE:view3d>" "${HHUOS_ROOT_DIR}/hdd0/img/bin/view3d"
        COMMAND /bin/cp -r "${CMAKE_BINARY_DIR}/beep" "${HHUOS_ROOT_DIR}/hdd0/img/user"
        COMMAND /bin/cp -r "${CMAKE_BINARY_DIR}/asciimation" "${HHUOS_ROOT_DIR}/hdd0/img/user"
        COMMAND /bin/cp -r "${CMAKE_BINARY_DIR}/books" "${HHUOS_ROOT_DIR}/hdd0/img/user"
        WORKING_DIRECTORY ${HHUOS_ROOT_DIR}/hdd0 COMMAND ${HHUOS_ROOT_DIR}/hdd0/build.sh
        DEPENDS asciimation music books shell asciimate battlespace beep bug cat color cp date demo dino echo head hexdump ip kill ls lvgl_demo membench mkdir mount mouse ping play ps pwd rm rmdir shutdown smbios touch tree uecho unmount uptime view3d)

add_custom_target(${PROJECT_NAME} DEPENDS asciimation music books shell asciimate battlespace beep bug cat color cp date demo dino echo head hexdump ip kill ls lvgl_demo membench mkdir mount mouse ping play ps  pwd rm rmdir shutdown smbios touch tree uecho unmount uptime view3d "${HHUOS_ROOT_DIR}/hdd0.img")
