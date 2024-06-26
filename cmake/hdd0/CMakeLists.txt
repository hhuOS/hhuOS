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
        WORKING_DIRECTORY "${HHUOS_ROOT_DIR}/disk/hdd0/"
        COMMAND /bin/mkdir -p "bin/"
        COMMAND /bin/mkdir -p "user/"
        COMMAND /bin/mkdir -p "media/floppy/"
        COMMAND /bin/mkdir -p "media/cdrom/"
        COMMAND /bin/cp "$<TARGET_FILE:shell>" "bin/shell"
        COMMAND /bin/cp "$<TARGET_FILE:asciimate>" "bin/asciimate"
        COMMAND /bin/cp "$<TARGET_FILE:battlespace>" "bin/battlespace"
        COMMAND /bin/cp "$<TARGET_FILE:beep>" "bin/beep"
        COMMAND /bin/cp "$<TARGET_FILE:bug>" "bin/bug"
        COMMAND /bin/cp "$<TARGET_FILE:cat>" "bin/cat"
        COMMAND /bin/cp "$<TARGET_FILE:cp>" "bin/cp"
        COMMAND /bin/cp "$<TARGET_FILE:date>" "bin/date"
        COMMAND /bin/cp "$<TARGET_FILE:demo>" "bin/demo"
        COMMAND /bin/cp "$<TARGET_FILE:dino>" "bin/dino"
        COMMAND /bin/cp "$<TARGET_FILE:echo>" "bin/echo"
        COMMAND /bin/cp "$<TARGET_FILE:head>" "bin/head"
        COMMAND /bin/cp "$<TARGET_FILE:hexdump>" "bin/hexdump"
        COMMAND /bin/cp "$<TARGET_FILE:ip>" "bin/ip"
        COMMAND /bin/cp "$<TARGET_FILE:kill>" "bin/kill"
        COMMAND /bin/cp "$<TARGET_FILE:ls>" "bin/ls"
        COMMAND /bin/cp "$<TARGET_FILE:membench>" "bin/membench"
        COMMAND /bin/cp "$<TARGET_FILE:mkdir>" "bin/mkdir"
        COMMAND /bin/cp "$<TARGET_FILE:mount>" "bin/mount"
        COMMAND /bin/cp "$<TARGET_FILE:nettest>" "bin/nettest"
        COMMAND /bin/cp "$<TARGET_FILE:ping>" "bin/ping"
        COMMAND /bin/cp "$<TARGET_FILE:play>" "bin/play"
        COMMAND /bin/cp "$<TARGET_FILE:ps>" "bin/ps"
        COMMAND /bin/cp "$<TARGET_FILE:pwd>" "bin/pwd"
        COMMAND /bin/cp "$<TARGET_FILE:rm>" "bin/rm"
        COMMAND /bin/cp "$<TARGET_FILE:rmdir>" "bin/rmdir"
        COMMAND /bin/cp "$<TARGET_FILE:shutdown>" "bin/shutdown"
        COMMAND /bin/cp "$<TARGET_FILE:smbios>" "bin/smbios"
        COMMAND /bin/cp "$<TARGET_FILE:touch>" "bin/touch"
        COMMAND /bin/cp "$<TARGET_FILE:tree>" "bin/tree"
        COMMAND /bin/cp "$<TARGET_FILE:uecho>" "bin/uecho"
        COMMAND /bin/cp "$<TARGET_FILE:unmount>" "bin/unmount"
        COMMAND /bin/cp "$<TARGET_FILE:uptime>" "bin/uptime"
        COMMAND /bin/cp "$<TARGET_FILE:view3d>" "bin/view3d"
        COMMAND /bin/cp -r "${CMAKE_BINARY_DIR}/asciimation" "user"
        COMMAND /bin/cp -r "${CMAKE_BINARY_DIR}/beep" "user"
        COMMAND /bin/cp -r "${CMAKE_BINARY_DIR}/books" "user"
        COMMAND /bin/cp -r "${CMAKE_BINARY_DIR}/wav" "user"
        COMMAND mkfs.fat -C "${CMAKE_BINARY_DIR}/part.img" $<IF:$<CONFIG:Debug>,63488,30720>
        COMMAND mcopy -s -o -i "${CMAKE_BINARY_DIR}/part.img" * ::/
        COMMAND fallocate -l 1M "${CMAKE_BINARY_DIR}/fill.img"
        COMMAND /bin/cat "${CMAKE_BINARY_DIR}/fill.img" "${CMAKE_BINARY_DIR}/part.img" "${CMAKE_BINARY_DIR}/fill.img" > "${HHUOS_ROOT_DIR}/hdd0.img"
        COMMAND /bin/rm "${CMAKE_BINARY_DIR}/part.img" "${CMAKE_BINARY_DIR}/fill.img"
        COMMAND /bin/echo -e "'o\\nn\\np\\n1\\n2048\\n131071\\nt\\ne\\nw\\n'" | fdisk "${HHUOS_ROOT_DIR}/hdd0.img"
        DEPENDS asciimation-star-wars books-gutenberg shell asciimate battlespace beep bug cat cp date demo dino echo head hexdump ip kill ls membench mkdir mount nettest ping play ps pwd rm rmdir shutdown smbios touch tree uecho unmount uptime view3d)

add_custom_target(${PROJECT_NAME} DEPENDS asciimation-star-wars books-gutenberg music shell asciimate battlespace beep bug cat cp date demo dino echo head hexdump ip kill ls membench mkdir mount nettest ping play ps  pwd rm rmdir shutdown smbios touch tree uecho unmount uptime view3d "${HHUOS_ROOT_DIR}/hdd0.img")
