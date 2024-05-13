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

project(limine)
message(STATUS "Project " ${PROJECT_NAME})

add_custom_command(OUTPUT "${HHUOS_ROOT_DIR}/hhuOS-limine.iso"
        WORKING_DIRECTORY "${HHUOS_ROOT_DIR}/loader/limine/"
        COMMAND /bin/mkdir -p "iso"
        COMMAND /bin/cp "$<TARGET_FILE:system>" "${HHUOS_ROOT_DIR}/loader/limine/iso/kernel.elf"
        COMMAND "${HHUOS_ROOT_DIR}/loader/limine/build.sh"
        COMMAND /bin/mv "${HHUOS_ROOT_DIR}/loader/limine/hhuOS-limine.iso" "${HHUOS_ROOT_DIR}"
        DEPENDS system floppy0 hdd0)

add_custom_command(OUTPUT "${HHUOS_ROOT_DIR}/hhuOS-limine-vdd.iso"
        WORKING_DIRECTORY "${HHUOS_ROOT_DIR}/loader/limine/"
        COMMAND /bin/mkdir -p "iso"
        COMMAND /bin/cp "$<TARGET_FILE:system>" "${HHUOS_ROOT_DIR}/loader/limine/iso/kernel.elf"
        COMMAND "${HHUOS_ROOT_DIR}/loader/limine/build.sh" "--with-virtual-hdd"
        COMMAND /bin/mv "${HHUOS_ROOT_DIR}/loader/limine/hhuOS-limine.iso" "${HHUOS_ROOT_DIR}/hhuOS-limine-vdd.iso"
        DEPENDS system floppy0 hdd0)

add_custom_target(${PROJECT_NAME} DEPENDS system floppy0 hdd0 "${HHUOS_ROOT_DIR}/hhuOS-limine.iso")
add_custom_target(${PROJECT_NAME}-vdd DEPENDS system floppy0 hdd0 "${HHUOS_ROOT_DIR}/hhuOS-limine-vdd.iso")