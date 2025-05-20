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

project(peanut-gb)
message(STATUS "Project " ${PROJECT_NAME})

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/gameboy"
        COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/gameboy")

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/gameboy/2048.gb"
        COMMAND wget "https://sanqui.net/etc/2048.gb" -O "${CMAKE_BINARY_DIR}/gameboy/2048.gb"
        COMMAND /bin/touch -a -m "${CMAKE_BINARY_DIR}/gameboy/2048.gb"
        DEPENDS "${CMAKE_BINARY_DIR}/gameboy")

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/gameboy/5-mazes.gb"
        COMMAND wget "https://hh3.gbdev.io/static/database-gb/entries/5-mazes/5%20mazes.gb" -O "${CMAKE_BINARY_DIR}/gameboy/5-mazes.gb"
        COMMAND /bin/touch -a -m "${CMAKE_BINARY_DIR}/gameboy/5-mazes.gb"
        DEPENDS "${CMAKE_BINARY_DIR}/gameboy")

add_custom_target(gameboy-roms DEPENDS
        "${CMAKE_BINARY_DIR}/gameboy/2048.gb"
        "${CMAKE_BINARY_DIR}/gameboy/5-mazes.gb")

include_directories(${HHUOS_SRC_DIR} ${HHUOS_SRC_DIR}/lib/libc ${HHUOS_SRC_DIR}/lib/util)

add_compile_options(
        -Wno-switch
)

set(SOURCE_FILES
        ${HHUOS_SRC_DIR}/application/peanut-gb/peanut_gb.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} lib.user.runtime lib.user.base lib.user.libc)