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

project(grub)
message(STATUS "Project " ${PROJECT_NAME})

set(GRUB_CONFIG "\
set timeout=0\\n\
set default=0\\n\
set menu_color_highlight=light-blue/light-gray\\n\
\\n\
if [ x$feature_all_video_module = xy ]; then\\n\
    insmod all_video\\n\
else\\n\
    insmod efi_gop\\n\
    insmod efi_uga\\n\
    insmod ieee1275_fb\\n\
    insmod vbe\\n\
    insmod vga\\n\
    insmod video_bochs\\n\
    insmod video_cirrus\\n\
fi\\n\
\\n\
menuentry \"hhuOS\" {\\n\
    multiboot2 /boot/hhuOS/kernel.elf log_level=inf log_ports=COM1 root=ata0p0,Filesystem::Fat::FatDriver apic=true bios=true apm=true vbe=true\\n\
}")

set(GRUB_VDD_CONFIG "\
set timeout=0\\n\
set default=0\\n\
set menu_color_highlight=light-blue/light-gray\\n\
\\n\
if [ x$feature_all_video_module = xy ]; then\\n\
    insmod all_video\\n\
else\\n\
    insmod efi_gop\\n\
    insmod efi_uga\\n\
    insmod ieee1275_fb\\n\
    insmod vbe\\n\
    insmod vga\\n\
    insmod video_bochs\\n\
    insmod video_cirrus\\n\
fi\\n\
\\n\
menuentry \"hhuOS\" {\\n\
    multiboot2 /boot/hhuOS/kernel.elf log_level=inf log_ports=COM1 root=vdd0p0,Filesystem::Fat::FatDriver apic=true bios=true apm=true vbe=true\\n\
    module2 /boot/hhuOS/hdd0.img vdd0\\n\
}")

add_custom_command(OUTPUT "${HHUOS_ROOT_DIR}/hhuOS-grub.iso"
        COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/grub/iso/boot/grub"
        COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/grub/iso/boot/hhuOS"
        COMMAND /bin/echo -e "'${GRUB_CONFIG}'" | sed -r "s/[\\\\]+//g" > "${CMAKE_BINARY_DIR}/grub/iso/boot/grub/grub.cfg"
        COMMAND /bin/cp "$<TARGET_FILE:system>" "${CMAKE_BINARY_DIR}/grub/iso/boot/hhuOS/kernel.elf"
        COMMAND /bin/rm -f "${CMAKE_BINARY_DIR}/grub/iso/boot/hhuOS/hdd0.img"
        COMMAND grub-mkrescue "/usr/lib/grub/i386-pc" "/usr/lib/grub/i386-efi" -o "${HHUOS_ROOT_DIR}/hhuOS-grub.iso" "${CMAKE_BINARY_DIR}/grub/iso/"
        DEPENDS system floppy0 hdd0)

add_custom_command(OUTPUT "${HHUOS_ROOT_DIR}/hhuOS-grub-vdd.iso"
        COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/grub/iso/boot/grub"
        COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/grub/iso/boot/hhuOS"
        COMMAND /bin/echo -e "'${GRUB_VDD_CONFIG}'" | sed -r "s/[\\\\]+//g" > "${CMAKE_BINARY_DIR}/grub/iso/boot/grub/grub.cfg"
        COMMAND /bin/cp "$<TARGET_FILE:system>" "${CMAKE_BINARY_DIR}/grub/iso/boot/hhuOS/kernel.elf"
        COMMAND /bin/cp "${HHUOS_ROOT_DIR}/hdd0.img" "${CMAKE_BINARY_DIR}/grub/iso/boot/hhuOS/hdd0.img"
        COMMAND grub-mkrescue "/usr/lib/grub/i386-pc" "/usr/lib/grub/i386-efi" -o "${HHUOS_ROOT_DIR}/hhuOS-grub-vdd.iso" "${CMAKE_BINARY_DIR}/grub/iso/"
        DEPENDS system floppy0 hdd0)

add_custom_target(${PROJECT_NAME} DEPENDS system floppy0 hdd0 "${HHUOS_ROOT_DIR}/hhuOS-grub.iso")
add_custom_target(${PROJECT_NAME}-vdd DEPENDS system floppy0 hdd0 "${HHUOS_ROOT_DIR}/hhuOS-grub-vdd.iso")