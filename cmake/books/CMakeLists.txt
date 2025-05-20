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

# CMake does not like it, if the project name is part of the target path.
# So we cannot name the project 'books' while saving files to '${CMAKE_BINARY_DIR}/books/'.
project(books-gutenberg)
message(STATUS "Project " ${PROJECT_NAME})

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/books/"
        COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/books/")

if ($ENV{HHUOS_REDISTRIBUTABLE_BUILD})
    add_custom_target(${PROJECT_NAME} DEPENDS
            "${CMAKE_BINARY_DIR}/books/")
else()
    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/books/alice.txt"
            COMMAND wget "https://www.gutenberg.org/files/11/11-0.txt" -qO - | recode -f utf-8..ascii > "${CMAKE_BINARY_DIR}/books/alice.txt"
            DEPENDS "${CMAKE_BINARY_DIR}/books/")

    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/books/peter_pan.txt"
            COMMAND wget "https://www.gutenberg.org/files/16/16-0.txt" -qO - | recode -f utf-8..ascii >  "${CMAKE_BINARY_DIR}/books/peter_pan.txt"
            DEPENDS "${CMAKE_BINARY_DIR}/books/")

    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/books/sleepy_hollow.txt"
            COMMAND wget "https://www.gutenberg.org/files/41/41-0.txt" -qO - | recode -f utf-8..ascii >  "${CMAKE_BINARY_DIR}/books/sleepy_hollow.txt"
            DEPENDS "${CMAKE_BINARY_DIR}/books/")

    add_custom_target(${PROJECT_NAME} DEPENDS
            "${CMAKE_BINARY_DIR}/books/alice.txt"
            "${CMAKE_BINARY_DIR}/books/peter_pan.txt"
            "${CMAKE_BINARY_DIR}/books/sleepy_hollow.txt")
endif()