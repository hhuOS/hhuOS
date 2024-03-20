#!/bin/bash

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

readonly LIMINE_VERSION="7.1.0"

if [[ "${1}" = "--with-virtual-hdd" ]]; then
  INCLUDE_HDD="true"
else
  INCLUDE_HDD="false"
fi

if [[ ! -f "iso/limine-uefi-cd.bin" || ! -f "iso/limine-bios-cd.bin" || ! -f "iso/limine-bios.sys" ||! -f "iso/EFI/BOOT/BOOTIA32.EFI" || ! -f "iso/EFI/BOOT/BOOTX64.EFI" || ! -f "limine" ]]; then
  wget -O limine.zip "https://github.com/limine-bootloader/limine/archive/refs/tags/v${LIMINE_VERSION}-binary.zip" || exit 1
  unzip limine.zip || exit 1
  cd "limine-${LIMINE_VERSION}-binary" || exit 1

  make || exit 1
  cp "limine" ".." || exit 1

  cp "limine-uefi-cd.bin" "../iso" || exit 1
  cp "limine-bios-cd.bin" "../iso" || exit 1
  cp "limine-bios.sys" "../iso" || exit 1

  mkdir -p "../iso/EFI/BOOT" || exit 1
  cp "BOOTIA32.EFI" "../iso/EFI/BOOT" || exit 1
  cp "BOOTX64.EFI" "../iso/EFI/BOOT" || exit 1

  cd .. || exit 1
  rm -r limine.zip limine-${LIMINE_VERSION}-binary || exit 1
fi

if [[ "${INCLUDE_HDD}" = "true" ]]; then
  cp "limine_vdd.cfg" "iso/limine.cfg" || exit 1
  cp "../../hdd0.img" "iso" || exit 1
else
  cp "limine.cfg" "iso/limine.cfg" || exit 1
  rm -f "iso/hdd0.img" || exit 1
fi

cd iso || exit 1
xorriso -as mkisofs -b limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot limine-uefi-cd.bin -efi-boot-part --efi-boot-image --protective-msdos-label . -o ../hhuOS-limine.iso || exit 1
cd .. || exit 1
./limine bios-install hhuOS-limine.iso || exit 1
