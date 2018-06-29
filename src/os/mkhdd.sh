#!/bin/bash

rm -f hdd0.img > /dev/null

fallocate -l 40M hdd0.img > /dev/null

(echo n; echo p; echo 1;echo "2048"; echo ""; echo t; echo 58; echo w) | fdisk hdd0.img > /dev/null

fallocate -l 39M hdd0p1 > /dev/null

mkfs.fat -F32 -S512 hdd0p1 > /dev/null

find "../hdd" -type d | while read dir; do
	current_dir=$(realpath --relative-to="../hdd" "$dir")
	[ -z "$current_dir" ] && continue
	mmd -i "./hdd0p1" "::$current_dir" &> /dev/null
done

find "../hdd" -type f | while read file; do
	current_file=$(realpath --relative-to="../hdd" "$file")
	mcopy -i "./hdd0p1" "$file" "::$current_file" &> /dev/null
done

dd if=hdd0p1 of=hdd0.img bs=512 seek=2048 &> /dev/null

rm hdd0p1 > /dev/null
