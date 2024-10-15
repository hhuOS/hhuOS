# Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

cmake_minimum_required(VERSION 3.14)

project(application)
message(STATUS "Project " ${PROJECT_NAME})

include_directories(${HHUOS_SRC_DIR})

add_compile_options(-I ${HHUOS_SRC_DIR}/lib/libc)

# Add subdirectories
add_subdirectory(shell)
add_subdirectory(asciimate)
add_subdirectory(battlespace)
add_subdirectory(bug)
add_subdirectory(beep)
add_subdirectory(cat)
add_subdirectory(cp)
add_subdirectory(ctest)
add_subdirectory(demo)
add_subdirectory(date)
add_subdirectory(dino)
add_subdirectory(doom)
add_subdirectory(echo)
add_subdirectory(head)
add_subdirectory(hexdump)
add_subdirectory(ip)
add_subdirectory(keyboard)
add_subdirectory(kill)
add_subdirectory(ls)
add_subdirectory(membench)
add_subdirectory(mkdir)
add_subdirectory(mount)
add_subdirectory(nettest)
add_subdirectory(peanut-gb)
add_subdirectory(ping)
add_subdirectory(play)
add_subdirectory(portablegl)
add_subdirectory(ps)
add_subdirectory(pwd)
add_subdirectory(quake)
add_subdirectory(rm)
add_subdirectory(rmdir)
add_subdirectory(shutdown)
add_subdirectory(smbios)
add_subdirectory(tinygl)
add_subdirectory(touch)
add_subdirectory(tree)
add_subdirectory(uecho)
add_subdirectory(unmount)
add_subdirectory(uptime)
add_subdirectory(view3d)
