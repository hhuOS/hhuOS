# Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
# Institute of Computer Science, Department Operating Systems
# Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
# Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
# This project has been supported by several students.
# A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

project(floppy0)
message(STATUS "Project " ${PROJECT_NAME})

add_custom_command(OUTPUT "${HHUOS_ROOT_DIR}/floppy0.img"
        WORKING_DIRECTORY "${HHUOS_ROOT_DIR}/disk/"
        COMMAND /bin/mkdir -p "floppy0/bin/"
        COMMAND /bin/cp "$<TARGET_FILE:echo>" "floppy0/bin/echo"
        COMMAND /bin/cp -r "${CMAKE_BINARY_DIR}/books" "floppy0/"
        COMMAND /bin/rm -f "${HHUOS_ROOT_DIR}/floppy0.img"
        COMMAND mkfs.fat -F 12 -C "${HHUOS_ROOT_DIR}/floppy0.img" 2880
        COMMAND mcopy -s -o -i "${HHUOS_ROOT_DIR}/floppy0.img" "floppy0/*" ::/
        DEPENDS books-gutenberg echo)

add_custom_target(${PROJECT_NAME} DEPENDS books-gutenberg echo "${HHUOS_ROOT_DIR}/floppy0.img")