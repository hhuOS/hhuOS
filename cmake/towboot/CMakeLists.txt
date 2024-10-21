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

project(towboot)
message(STATUS "Project " ${PROJECT_NAME})

set(TOWBOOT_VERSION "0.9.1")

set(TOWBOOT_CONFIG "\
default = \"hhuOS\"\\n\
timeout = 0\\n\
log_level = \"info\"\\n\
\\n\
[entries]\\n\
[entries.hhuOS]\\n\
    name = \"hhuOS\"\\n\
    image = \"hhuOS/kernel.elf\"\\n\
    argv = \"log_level=inf log_ports=COM1 root=ata1p0,Filesystem::Fat::FatDriver apic=true bios=false apm=false vbe=false\"")

set(TOWBOOT_VDD_CONFIG "\
default = \"hhuOS\"\\n\
timeout = 0\\n\
log_level = \"info\"\\n\
\\n\
[entries]\\n\
[entries.hhuOS]\\n\
    name = \"hhuOS\"\\n\
    image = \"hhuOS/kernel.elf\"\\n\
    argv = \"log_level=inf log_ports=COM1 root=vdd0p0,Filesystem::Fat::FatDriver apic=true bios=false apm=false vbe=false\"\\n\
    modules = [ { image = \"hhuOS/hdd0.img\", argv = \"vdd0\" } ]")

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/towboot/towbootctl"
        COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/towboot"
        COMMAND wget -O "${CMAKE_BINARY_DIR}/towboot/towbootctl" "https://github.com/hhuOS/towboot/releases/download/v${TOWBOOT_VERSION}/towbootctl-v${TOWBOOT_VERSION}-x86_64-linux"
        COMMAND chmod +x "${CMAKE_BINARY_DIR}/towboot/towbootctl")

add_custom_command(OUTPUT "${HHUOS_ROOT_DIR}/hhuOS-towboot.img"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/towboot/"
        COMMAND /bin/mkdir -p "hhuOS"
        COMMAND /bin/cp "$<TARGET_FILE:system>" "hhuOS/kernel.elf"
        COMMAND /bin/echo -e "'${TOWBOOT_CONFIG}'" | sed -r "s/[\\\\]+//g" > "towboot.toml"
        COMMAND ./towbootctl image --target "${HHUOS_ROOT_DIR}/hhuOS-towboot.img" -- -config "towboot.toml"
        DEPENDS system floppy0 hdd0 "${CMAKE_BINARY_DIR}/towboot/towbootctl")

add_custom_command(OUTPUT "${HHUOS_ROOT_DIR}/hhuOS-towboot-vdd.img"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/towboot/"
        COMMAND /bin/mkdir -p "hhuOS"
        COMMAND /bin/cp "$<TARGET_FILE:system>" "hhuOS/kernel.elf"
        COMMAND /bin/cp "${HHUOS_ROOT_DIR}/hdd0.img" "hhuOS/hdd0.img"
        COMMAND /bin/echo -e "'${TOWBOOT_VDD_CONFIG}'" | sed -r "s/[\\\\]+//g" > "towboot.toml"
        COMMAND ./towbootctl image --target "${HHUOS_ROOT_DIR}/hhuOS-towboot-vdd.img" -- -config "towboot.toml"
        DEPENDS system floppy0 hdd0 "${CMAKE_BINARY_DIR}/towboot/towbootctl")

add_custom_target(${PROJECT_NAME} DEPENDS system floppy0 hdd0 "${HHUOS_ROOT_DIR}/hhuOS-towboot.img")
add_custom_target(${PROJECT_NAME}-vdd DEPENDS system floppy0 hdd0 "${HHUOS_ROOT_DIR}/hhuOS-towboot-vdd.img")
