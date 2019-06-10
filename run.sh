#!/bin/bash

check_file() {
    local file=$1
    
    if [ ! -f $file ]; then
        printf "File '%s' does not exist! Did you run build.sh?\n" "${file}"
        exit 1
    fi
}

check_file hhuOS.iso
check_file hdd0.img

qemu-system-i386 -machine pc,accel=kvm,kernel-irqchip=off \
    -boot d -cdrom hhuOS.iso -m 128M -k de -vga std -monitor stdio \
    -cpu pentium,+sse,+sse2 \
    -rtc base=localtime,clock=host \
    -drive index=0,if=floppy \
    -drive format=raw,file=hdd0.img,if=none,id=disk0 \
    -device ich9-ahci,id=ahci \
    -device ide-drive,drive=disk0,bus=ahci.0 \
    -device sb16,irq=10,dma=1 \
    -soundhw pcspk \
    -netdev user,id=eth0,hostfwd=tcp::8821-:8821 \
    -device e1000,netdev=eth0 \
    -object filter-dump,id=filter0,netdev=eth0,file=eth0.dump
