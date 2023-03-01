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

cmake_minimum_required(VERSION 3.14)

project(books)
message(STATUS "Project " ${PROJECT_NAME})

if ($ENV{HHUOS_REDISTRIBUTABLE_BUILD})
    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/books/"
        COMMAND COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/books/")

    add_custom_target(${PROJECT_NAME} DEPENDS
            "${CMAKE_BINARY_DIR}/books/")
else()
    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/books/alice.txt"
            COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/books/"
            COMMAND wget https://www.gutenberg.org/files/11/11-0.txt -O "${CMAKE_BINARY_DIR}/books/alice.txt")

    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/books/peter_pan.txt"
            COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/books/"
            COMMAND wget https://www.gutenberg.org/files/16/16-0.txt -O "${CMAKE_BINARY_DIR}/books/peter_pan.txt")

    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/books/sleepy_hollow.txt"
            COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/books/"
            COMMAND wget https://www.gutenberg.org/files/41/41-0.txt -O "${CMAKE_BINARY_DIR}/books/sleepy_hollow.txt")

    add_custom_target(${PROJECT_NAME} DEPENDS
            "${CMAKE_BINARY_DIR}/books/alice.txt"
            "${CMAKE_BINARY_DIR}/books/peter_pan.txt"
            "${CMAKE_BINARY_DIR}/books/sleepy_hollow.txt")
endif()