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

readonly TOWBOOT_VERSION="0.7.1"

if [[ "${1}" = "--with-virtual-hdd" ]]; then
  INCLUDE_HDD="true"
else
  INCLUDE_HDD="false"
fi

FILE_LIST=("towboot-ia32.efi" "towboot-x64.efi" "hhuOS.bin" "towboot.toml")
if [[ "${INCLUDE_HDD}" = "true" ]]; then
  FILE_LIST+=("../../hdd0.img")
fi

echo "${FILE_LIST[@]}"

if [[ ! -f "towboot-ia32.efi" || ! -f "towboot-x64.efi" ]]; then
  wget -O towboot-ia32.efi "https://github.com/hhuOS/towboot/releases/download/v${TOWBOOT_VERSION}/towboot-v${TOWBOOT_VERSION}-i686.efi" || exit 1
  wget -O towboot-x64.efi "https://github.com/hhuOS/towboot/releases/download/v${TOWBOOT_VERSION}/towboot-v${TOWBOOT_VERSION}-x86_64.efi" || exit 1
fi

SIZE=0;
for file in ${FILE_LIST[@]}; do
  SIZE=$(($SIZE + $(wc -c ${file} | cut -d ' ' -f 1)))
done

readonly SECTORS=$(((${SIZE} / 512) + 2048))

mformat -i part.img -C -T ${SECTORS} || exit 1
mmd -i part.img efi || exit 1
mmd -i part.img efi/boot || exit 1
mcopy -i part.img towboot-ia32.efi ::efi/boot/bootia32.efi || exit 1
mcopy -i part.img towboot-x64.efi ::efi/boot/bootx64.efi || exit 1
mcopy -i part.img hhuOS.bin :: || exit 1

if [[ "${INCLUDE_HDD}" = "true" ]]; then
  mcopy -i part.img towboot_vdd.toml ::towboot.toml || exit 1
  mcopy -i part.img ../../hdd0.img :: || exit 1
else
  mcopy -i part.img towboot.toml :: || exit 1
fi

fallocate -l 1M fill.img || exit 1
cat fill.img part.img fill.img > hhuOS-towboot.img || exit 1
echo -e "g\\nn\\n1\\n2048\\n+${SECTORS}\\nt\\n1\\nw\\n" | fdisk hhuOS-towboot.img || exit 1

rm -f fill.img part.img || exit 1
