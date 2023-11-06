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

if [[ "${1}" = "--with-virtual-hdd" ]]; then
  INCLUDE_HDD="true"
else
  INCLUDE_HDD="false"
fi

if [[ "${INCLUDE_HDD}" = "true" ]]; then
  cp "grub_vdd.cfg" "iso/boot/grub/grub.cfg" || exit 1
  cp "../../hdd0.img" "iso/boot" || exit 1
else
  cp "grub.cfg" "iso/boot/grub/grub.cfg"
  rm -f "iso/boot/hdd0.img"
fi

grub-mkrescue "/usr/lib/grub/i386-pc" "/usr/lib/grub/i386-efi" -o "hhuOS-grub.iso" "iso/"
