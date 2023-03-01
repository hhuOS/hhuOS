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


rm -f "../hdd0.img"

mkfs.fat -F 16 -C "part.img" 63488
mcopy -s -o -i "part.img" img/* ::/

fallocate -l 1M "fill.img"
cat fill.img part.img fill.img > "../hdd0.img"

rm part.img fill.img
echo -e "o\\nn\\np\\n1\\n2048\\n131071\\nt\\ne\\nw\\n" | fdisk "../hdd0.img"
