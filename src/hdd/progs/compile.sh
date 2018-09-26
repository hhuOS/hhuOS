# Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
# Heinrich-Heine University
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

#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Illegal number of arguments"
    exit 1
fi

if [ ! -f "$1.cpp" ]; then
    echo "File not found!"
    exit 1
fi

g++ -m32 -nostdlib -ffreestanding -fpic -O0 -Wl,--hash-style=sysv -o printf.o -c printf.cpp
g++ -m32 -shared -nostdlib -ffreestanding -fpic -O0 -Wl,--hash-style=sysv -o libprintf.so  printf.o
g++ -m32 -nostdlib -ffreestanding -fpic -O0 -Wl,--hash-style=sysv -o "$1".o -c "$1".cpp
g++ -m32 -nostdlib -ffreestanding -fpic -O0 -Wl,--hash-style=sysv -o "$1" "$1".o -lprintf -L.

rm printf.o
rm libprintf.so
rm "$1".o
mv "$1" ../bin/"$1"
