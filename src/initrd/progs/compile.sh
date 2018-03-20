#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Illegal number of arguments"
    exit 1
fi

if [ ! -f "$1.cc" ]; then
    echo "File not found!"
    exit 1
fi

g++ -m32 -nostdlib -ffreestanding -fpic -O0 -Wl,--hash-style=sysv -o printf.o -c printf.cc
g++ -m32 -shared -nostdlib -ffreestanding -fpic -O0 -Wl,--hash-style=sysv -o libprintf.so  printf.o
g++ -m32 -nostdlib -ffreestanding -fpic -O0 -Wl,--hash-style=sysv -o "$1".o -c "$1".cc
g++ -m32 -nostdlib -ffreestanding -fpic -O0 -Wl,--hash-style=sysv -o "$1" "$1".o -lprintf -L.

rm printf.o
rm libprintf.so
rm "$1".o
mv "$1" ../bin/"$1"
