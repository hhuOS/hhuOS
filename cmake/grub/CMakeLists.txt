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

project(grub)
message(STATUS "Project " ${PROJECT_NAME})

add_custom_command(OUTPUT "${HHUOS_ROOT_DIR}/hhuOS.iso"
        COMMAND /bin/cp "$<TARGET_FILE:system>" "${HHUOS_ROOT_DIR}/loader/grub/boot/hhuOS.bin"
        COMMAND /bin/cp "${CMAKE_BINARY_DIR}/hhuOS.initrd" "${HHUOS_ROOT_DIR}/loader/grub/boot/hhuOS.initrd"
        COMMAND grub-mkrescue "/usr/lib/grub/i386-pc" "/usr/lib/grub/i386-efi" -o "${HHUOS_ROOT_DIR}/hhuOS.iso" "${HHUOS_ROOT_DIR}/loader/grub/"
        DEPENDS system initrd floppy0 hdd0)

add_custom_target(${PROJECT_NAME} DEPENDS system initrd floppy0 hdd0 "${HHUOS_ROOT_DIR}/hhuOS.iso")