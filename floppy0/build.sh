#!/bin/bash

rm -f "../floppy0.img"
mkfs.fat -C "../floppy0.img" 1440
mcopy -o -i "../floppy0.img" img/* ::/
