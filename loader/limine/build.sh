#!/bin/bash

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

readonly LIMINE_VERSION="4.20230317.0"

if [[ ! -f "iso/limine-cd-efi.bin" || ! -f "iso/limine-cd.bin" || ! -f "iso/limine.sys" || ! -f "limine-deploy" ]]; then
  wget -O limine.zip "https://github.com/limine-bootloader/limine/archive/refs/tags/v${LIMINE_VERSION}-binary.zip" || exit 1
  unzip limine.zip || exit 1
  mv "limine-${LIMINE_VERSION}-binary/limine-cd-efi.bin" "iso" || exit 1
  mv "limine-${LIMINE_VERSION}-binary/limine-cd.bin" "iso" || exit 1
  mv "limine-${LIMINE_VERSION}-binary/limine.sys" "iso" || exit 1
  gcc -o limine-deploy "limine-${LIMINE_VERSION}-binary/limine-deploy.c" || exit 1
  rm -r limine.zip limine-${LIMINE_VERSION}-binary || exit 1
fi

cd iso || exit 1
xorriso -as mkisofs -b limine-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot limine-cd-efi.bin -efi-boot-part --efi-boot-image --protective-msdos-label . -o ../hhuOS-limine.iso || exit 1
cd .. || exit 1
./limine-deploy hhuOS-limine.iso || exit 1