# Copyright (C) 2019 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

cmake_minimum_required(VERSION 3.7)

target_sources(lib PUBLIC
        ${HHUOS_SRC_DIR}/lib/libc/ctype.c
        ${HHUOS_SRC_DIR}/lib/libc/math.c
        ${HHUOS_SRC_DIR}/lib/libc/stdlib.c
        ${HHUOS_SRC_DIR}/lib/libc/string.c
        ${HHUOS_SRC_DIR}/lib/libc/printf.cpp
        ${HHUOS_SRC_DIR}/lib/libc/sprintf.cpp
        ${HHUOS_SRC_DIR}/lib/libc/snprintf.cpp
        ${HHUOS_SRC_DIR}/lib/libc/strformat.cpp
        ${HHUOS_SRC_DIR}/lib/libc/system_interface.cpp)