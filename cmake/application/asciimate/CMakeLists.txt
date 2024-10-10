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

project(asciimate)
message(STATUS "Project " ${PROJECT_NAME})

# CMake does not like it, if the target name is part of the target path.
# So we cannot name the target 'asciimation' while saving files to '${CMAKE_BINARY_DIR}/asciimation/'.
add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/asciimation/"
        COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/asciimation/")

if ($ENV{HHUOS_REDISTRIBUTABLE_BUILD})
    add_custom_target(asciimation-star-wars DEPENDS
            "${CMAKE_BINARY_DIR}/asciimation/")
else()
    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/asciimation/star_wars.txt"
            COMMAND wget "https://www.asciimation.co.nz/asciimation/SwPlay.jar"
            COMMAND unzip -u "SwPlay.jar" -d "star_wars"
            COMMAND /bin/echo 14,67 | /bin/cat - "star_wars/data/sw1.txt" >> "${CMAKE_BINARY_DIR}/asciimation/star_wars.txt"
            DEPENDS "${CMAKE_BINARY_DIR}/asciimation/")

    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/asciimation/jar_jar.txt"
            COMMAND wget "https://www.asciimation.co.nz/asciimation/jjplay.jar"
            COMMAND unzip -u "jjplay.jar" -d "jar_jar"
            COMMAND /bin/echo 23,81 | /bin/cat - "jar_jar/data/jj1.txt" >> "${CMAKE_BINARY_DIR}/asciimation/jar_jar.txt"
            DEPENDS "${CMAKE_BINARY_DIR}/asciimation/")

    add_custom_target(asciimation-star-wars DEPENDS
            "${CMAKE_BINARY_DIR}/asciimation/star_wars.txt"
            "${CMAKE_BINARY_DIR}/asciimation/jar_jar.txt")
endif()

include_directories(${HHUOS_SRC_DIR})

# Set source files
set(SOURCE_FILES
        ${HHUOS_SRC_DIR}/application/asciimate/asciimate.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} lib.user.runtime lib.user.base lib.user.graphic)