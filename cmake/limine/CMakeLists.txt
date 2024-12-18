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

set(LIMINE_VERSION "8.6.0")

set(LIMINE_CONFIG "\
default_entry: 1\\n\
timeout: 0\\n\
verbose: yes\\n\
\\n\
/hhuOS\\n\
    comment: Boot hhuOS.\\n\
    protocol: multiboot2\\n\
    kernel_path: boot():/hhuOS/kernel.elf\\n\
    kernel_cmdline: log_level=inf log_ports=COM1 root=ata0p0,Filesystem::Fat::FatDriver apic=true bios=true apm=true vbe=true")

set(LIMINE_VDD_CONFIG "\
default_entry: 1\\n\
timeout: 0\\n\
verbose: yes\\n\
\\n\
/hhuOS\\n\
    comment: Boot hhuOS.\\n\
    protocol: multiboot2\\n\
    kernel_path: boot():/hhuOS/kernel.elf\\n\
    kernel_cmdline: log_level=inf log_ports=COM1 root=vdd0p0,Filesystem::Fat::FatDriver apic=true bios=true apm=true vbe=true\\n\
    module_path: boot():/hhuOS/hdd0.img\\n\
    module_string: vdd0")

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/limine/iso/limine-uefi-cd.bin" "${CMAKE_BINARY_DIR}/limine/iso/limine-bios-cd.bin" "${CMAKE_BINARY_DIR}/limine/iso/limine-bios.sys" "${CMAKE_BINARY_DIR}/limine/iso/EFI/BOOT/BOOTIA32.EFI" "${CMAKE_BINARY_DIR}/limine/iso/EFI/BOOT/BOOTX64.EFI"
        COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/limine/iso/EFI/BOOT"
        COMMAND wget "-O" "${CMAKE_BINARY_DIR}/limine/limine.zip" "https://github.com/limine-bootloader/limine/archive/refs/tags/v${LIMINE_VERSION}-binary.zip"
        COMMAND unzip "-o" "${CMAKE_BINARY_DIR}/limine/limine.zip" -d "${CMAKE_BINARY_DIR}/limine/"
        COMMAND /bin/cp "${CMAKE_BINARY_DIR}/limine/limine-${LIMINE_VERSION}-binary/limine-uefi-cd.bin" "${CMAKE_BINARY_DIR}/limine/iso/"
        COMMAND /bin/cp "${CMAKE_BINARY_DIR}/limine/limine-${LIMINE_VERSION}-binary/limine-bios-cd.bin" "${CMAKE_BINARY_DIR}/limine/iso/"
        COMMAND /bin/cp "${CMAKE_BINARY_DIR}/limine/limine-${LIMINE_VERSION}-binary/limine-bios.sys" "${CMAKE_BINARY_DIR}/limine/iso/"
        COMMAND /bin/cp "${CMAKE_BINARY_DIR}/limine/limine-${LIMINE_VERSION}-binary/BOOTIA32.EFI" "${CMAKE_BINARY_DIR}/limine/iso/EFI/BOOT/"
        COMMAND /bin/cp "${CMAKE_BINARY_DIR}/limine/limine-${LIMINE_VERSION}-binary/BOOTX64.EFI" "${CMAKE_BINARY_DIR}/limine/iso/EFI/BOOT/"
        COMMAND /bin/rm "${CMAKE_BINARY_DIR}/limine/limine.zip")

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/limine/limine"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/limine/limine-${LIMINE_VERSION}-binary/"
        COMMAND make
        COMMAND /bin/cp "limine" "../"
        COMMAND /bin/rm "-r" "../limine-${LIMINE_VERSION}-binary/"
        DEPENDS "${CMAKE_BINARY_DIR}/limine/iso/limine-uefi-cd.bin" "${CMAKE_BINARY_DIR}/limine/iso/limine-bios-cd.bin" "${CMAKE_BINARY_DIR}/limine/iso/limine-bios.sys" "${CMAKE_BINARY_DIR}/limine/iso/EFI/BOOT/BOOTIA32.EFI" "${CMAKE_BINARY_DIR}/limine/iso/EFI/BOOT/BOOTX64.EFI")

add_custom_command(OUTPUT "${HHUOS_ROOT_DIR}/hhuOS-limine.iso"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/limine/"
        COMMAND /bin/mkdir -p "iso/hhuOS"
        COMMAND /bin/cp "$<TARGET_FILE:system>" "iso/hhuOS/kernel.elf"
        COMMAND /bin/rm -f "iso/hhuOS/hdd0.img"
        COMMAND /bin/echo -e "'${LIMINE_CONFIG}'" | sed -r "s/[\\\\]+//g" > "iso/limine.conf"
        COMMAND xorriso -as mkisofs -b "limine-bios-cd.bin" -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot "limine-uefi-cd.bin" -efi-boot-part --efi-boot-image --protective-msdos-label "iso/" -o "${HHUOS_ROOT_DIR}/hhuOS-limine.iso"
        COMMAND ./limine "bios-install" "${HHUOS_ROOT_DIR}/hhuOS-limine.iso"
        DEPENDS system floppy0 hdd0 "${CMAKE_BINARY_DIR}/limine/limine" "${CMAKE_BINARY_DIR}/limine/iso/limine-uefi-cd.bin" "${CMAKE_BINARY_DIR}/limine/iso/limine-bios-cd.bin" "${CMAKE_BINARY_DIR}/limine/iso/limine-bios.sys" "${CMAKE_BINARY_DIR}/limine/iso/EFI/BOOT/BOOTIA32.EFI" "${CMAKE_BINARY_DIR}/limine/iso/EFI/BOOT/BOOTX64.EFI")

add_custom_command(OUTPUT "${HHUOS_ROOT_DIR}/hhuOS-limine-vdd.iso"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/limine/"
        COMMAND /bin/mkdir -p "iso/hhuOS"
        COMMAND /bin/cp "$<TARGET_FILE:system>" "iso/hhuOS/kernel.elf"
        COMMAND /bin/cp "${HHUOS_ROOT_DIR}/hdd0.img" "iso/hhuOS/hdd0.img"
        COMMAND /bin/echo -e "'${LIMINE_VDD_CONFIG}'" | sed -r "s/[\\\\]+//g" > "iso/limine.conf"
        COMMAND xorriso -as mkisofs -b "limine-bios-cd.bin" -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot "limine-uefi-cd.bin" -efi-boot-part --efi-boot-image --protective-msdos-label "iso/" -o "${HHUOS_ROOT_DIR}/hhuOS-limine-vdd.iso"
        COMMAND ./limine "bios-install" "${HHUOS_ROOT_DIR}/hhuOS-limine-vdd.iso"
        DEPENDS system floppy0 hdd0 "${CMAKE_BINARY_DIR}/limine/limine" "${CMAKE_BINARY_DIR}/limine/iso/limine-uefi-cd.bin" "${CMAKE_BINARY_DIR}/limine/iso/limine-bios-cd.bin" "${CMAKE_BINARY_DIR}/limine/iso/limine-bios.sys" "${CMAKE_BINARY_DIR}/limine/iso/EFI/BOOT/BOOTIA32.EFI" "${CMAKE_BINARY_DIR}/limine/iso/EFI/BOOT/BOOTX64.EFI")

add_custom_target(${PROJECT_NAME} DEPENDS system floppy0 hdd0 "${HHUOS_ROOT_DIR}/hhuOS-limine.iso")
add_custom_target(${PROJECT_NAME}-vdd DEPENDS system hdd0 "${HHUOS_ROOT_DIR}/hhuOS-limine-vdd.iso")