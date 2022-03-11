#!/bin/bash

readonly CONST_TOWBOOT_VERSION="0.3.0"
readonly FILE_LIST=("towboot-ia32.efi" "towboot-x64.efi" "hhuOS.bin" "hhuOS.initrd")

if [[ ! -f "towboot-ia32.efi" || ! -f "towboot-x64.efi" ]]; then
  wget -O towboot-ia32.efi "https://github.com/hhuOS/towboot/releases/download/v${CONST_TOWBOOT_VERSION}/towboot-v${CONST_TOWBOOT_VERSION}-i686.efi"
  wget -O towboot-x64.efi "https://github.com/hhuOS/towboot/releases/download/v${CONST_TOWBOOT_VERSION}/towboot-v${CONST_TOWBOOT_VERSION}-x86_64.efi"
fi

SIZE=0;
for file in ${FILE_LIST[@]}; do
  SIZE=$(($SIZE + $(wc -c ${file} | cut -d ' ' -f 1)))
done

readonly SECTORS=$(((${SIZE} / 512) + 200))

mformat -i part.img -C -T ${SECTORS} -h 1 -s ${SECTORS}
mmd -i part.img efi
mmd -i part.img efi/boot
mcopy -i part.img towboot-ia32.efi ::efi/boot/bootia32.efi
mcopy -i part.img towboot-x64.efi ::efi/boot/bootx64.efi
mcopy -i part.img towboot.toml ::
mcopy -i part.img hhuOS.bin ::
mcopy -i part.img hhuOS.initrd ::

fallocate -l 1M fill.img
cat fill.img part.img fill.img > hhuOS.img
echo -e "g\\nn\\n1\\n2048\\n+${SECTORS}\\nt\\n1\\nw\\n" | fdisk hhuOS.img

rm -f fill.img part.img
