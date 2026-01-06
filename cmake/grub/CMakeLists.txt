# Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
# Institute of Computer Science, Department Operating Systems
# Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
# Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
# This project has been supported by several students.
# A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

set(GRUB_FLOPPY_CONFIG "\
set timeout=0\\n\
set default=0\\n\
set menu_color_highlight=light-blue/light-gray\\n\
\\n\
menuentry \"hhuOS\" {\\n\
    multiboot2 /boot/hhuOS/kernel.elf log_level=inf log_ports=COM1 root=floppy0,Filesystem::Fat::FatDriver apic=true bios=true apm=true vbe=true\\n\
}")

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/grub/iso/boot/grub/" "${CMAKE_BINARY_DIR}/grub/iso/boot/hhuOS/kernel.elf"
        COMMAND /bin/rm -f "${CMAKE_BINARY_DIR}/grub/iso/boot/hhuOS/hdd0.img"
        COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/grub/iso/boot/grub"
        COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/grub/iso/boot/hhuOS"
        COMMAND /bin/cp "$<TARGET_FILE:system>" "${CMAKE_BINARY_DIR}/grub/iso/boot/hhuOS/kernel.elf"
        DEPENDS system)

add_custom_command(OUTPUT "${HHUOS_ROOT_DIR}/hhuOS-grub.iso"
        COMMAND /bin/echo -e "'${GRUB_CONFIG}'" | sed -r "s/[\\\\]+//g" > "${CMAKE_BINARY_DIR}/grub/iso/boot/grub/grub.cfg"
        COMMAND grub-mkrescue "/usr/lib/grub/i386-pc" "/usr/lib/grub/i386-efi" -o "${HHUOS_ROOT_DIR}/hhuOS-grub.iso" "${CMAKE_BINARY_DIR}/grub/iso/"
        DEPENDS  "${CMAKE_BINARY_DIR}/grub/iso/boot/grub/" "${CMAKE_BINARY_DIR}/grub/iso/boot/hhuOS/kernel.elf")

add_custom_command(OUTPUT "${HHUOS_ROOT_DIR}/hhuOS-grub-vdd.iso"
        COMMAND /bin/echo -e "'${GRUB_VDD_CONFIG}'" | sed -r "s/[\\\\]+//g" > "${CMAKE_BINARY_DIR}/grub/iso/boot/grub/grub.cfg"
        COMMAND /bin/cp "${HHUOS_ROOT_DIR}/hdd0.img" "${CMAKE_BINARY_DIR}/grub/iso/boot/hhuOS/hdd0.img"
        COMMAND grub-mkrescue "/usr/lib/grub/i386-pc" "/usr/lib/grub/i386-efi" -o "${HHUOS_ROOT_DIR}/hhuOS-grub-vdd.iso" "${CMAKE_BINARY_DIR}/grub/iso/"
        DEPENDS  "${CMAKE_BINARY_DIR}/grub/iso/boot/grub/" "${CMAKE_BINARY_DIR}/grub/iso/boot/hhuOS/kernel.elf" hdd0)

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/grub-floppy.img"
        COMMAND grub-mkimage -p /boot/grub -C auto -O i386-pc -o "${CMAKE_BINARY_DIR}/grub-floppy.img" "biosdisk" "part_msdos" "fat" "multiboot2" "configfile")

add_custom_command(OUTPUT "${HHUOS_ROOT_DIR}/hhuOS-grub-floppy.img"
        COMMAND /bin/echo -e "'${GRUB_FLOPPY_CONFIG}'" | sed -r "s/[\\\\]+//g" > "${CMAKE_BINARY_DIR}/grub/iso/boot/grub/grub.cfg"
        COMMAND /bin/cp "$<TARGET_FILE:system>" "${CMAKE_BINARY_DIR}/grub/iso/boot/hhuOS/kernel.elf"
        COMMAND dd if="/dev/zero" of="${HHUOS_ROOT_DIR}/hhuOS-grub-floppy.img" bs=512 count=2880
        COMMAND dd if="/usr/lib/grub/i386-pc/boot.img" of="${HHUOS_ROOT_DIR}/hhuOS-grub-floppy.img" conv=notrunc
        COMMAND dd if="${CMAKE_BINARY_DIR}/grub-floppy.img" of="${HHUOS_ROOT_DIR}/hhuOS-grub-floppy.img" conv=notrunc seek=1
        COMMAND mformat -i "${HHUOS_ROOT_DIR}/hhuOS-grub-floppy.img" -kR `ls --block-size=512 -s "${CMAKE_BINARY_DIR}/grub-floppy.img" | cut -d ' ' -f1`
        COMMAND mmd -i "${HHUOS_ROOT_DIR}/hhuOS-grub-floppy.img" "::/boot"
        COMMAND mmd -i "${HHUOS_ROOT_DIR}/hhuOS-grub-floppy.img" "::/boot/grub"
        COMMAND mmd -i "${HHUOS_ROOT_DIR}/hhuOS-grub-floppy.img" "::/boot/hhuOS"
        COMMAND mmd -i "${HHUOS_ROOT_DIR}/hhuOS-grub-floppy.img" "::/system"
        COMMAND mmd -i "${HHUOS_ROOT_DIR}/hhuOS-grub-floppy.img" "::/bin"
        COMMAND mcopy -i "${HHUOS_ROOT_DIR}/hhuOS-grub-floppy.img" "${CMAKE_BINARY_DIR}/grub/iso/boot/grub/grub.cfg" "::/boot/grub"
        COMMAND mcopy -i "${HHUOS_ROOT_DIR}/hhuOS-grub-floppy.img" "${CMAKE_BINARY_DIR}/grub/iso/boot/hhuOS/kernel.elf" "::/boot/hhuOS"
        COMMAND mcopy -i "${HHUOS_ROOT_DIR}/hhuOS-grub-floppy.img" "${HHUOS_ROOT_DIR}/disk/hdd0/system/banner.txt" "::/system"
        COMMAND mcopy -i "${HHUOS_ROOT_DIR}/hhuOS-grub-floppy.img" "$<TARGET_FILE:shell>" "::/bin"
        DEPENDS "${CMAKE_BINARY_DIR}/grub/iso/boot/grub/" "${CMAKE_BINARY_DIR}/grub/iso/boot/hhuOS/kernel.elf" "${CMAKE_BINARY_DIR}/grub-floppy.img" shell)

add_custom_target(${PROJECT_NAME} DEPENDS "${HHUOS_ROOT_DIR}/hhuOS-grub.iso" floppy0 hdd0)
add_custom_target(${PROJECT_NAME}-vdd DEPENDS "${HHUOS_ROOT_DIR}/hhuOS-grub-vdd.iso" floppy0 hdd0)
add_custom_target(${PROJECT_NAME}-floppy DEPENDS "${HHUOS_ROOT_DIR}/hhuOS-grub-floppy.img")