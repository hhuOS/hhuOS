#!/bin/bash

HHUOS_ROOT_PATH=$1

rm -f "${HHUOS_ROOT_PATH}/hdd0.img" > /dev/null

fallocate -l 40M "${HHUOS_ROOT_PATH}/hdd0.img" > /dev/null

(echo n; echo p; echo 1;echo "2048"; echo ""; echo t; echo 58; echo w) | fdisk "${HHUOS_ROOT_PATH}/hdd0.img" > /dev/null

fallocate -l 39M hdd0p1 > /dev/null

mkfs.fat -F32 -S512 hdd0p1 > /dev/null

find "${HHUOS_ROOT_PATH}/hdd/" -type d | while read dir; do
	current_dir=$(realpath --relative-to="${HHUOS_ROOT_PATH}/hdd/" "$dir")
	[ -z "$current_dir" ] && continue
	mmd -i "./hdd0p1" "::$current_dir" &> /dev/null
done

find "${HHUOS_ROOT_PATH}/hdd/" -type f | while read file; do
	current_file=$(realpath --relative-to="${HHUOS_ROOT_PATH}/hdd/" "$file")
	mcopy -i "./hdd0p1" "$file" "::$current_file" &> /dev/null
done

dd if=hdd0p1 of="${HHUOS_ROOT_PATH}/hdd0.img" bs=512 seek=2048 &> /dev/null

rm hdd0p1 > /dev/null
