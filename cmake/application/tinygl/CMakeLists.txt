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

project(tinygl)
message(STATUS "Project " ${PROJECT_NAME})

include_directories(${HHUOS_SRC_DIR})

add_compile_options(-Wno-unused-function -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-fallthrough)

# Set source files
set(SOURCE_FILES
        ${HHUOS_SRC_DIR}/application/tinygl/tinygl.cpp
        ${HHUOS_SRC_DIR}/application/tinygl/util.cpp
        ${HHUOS_SRC_DIR}/application/tinygl/cubes.cpp
        ${HHUOS_SRC_DIR}/application/tinygl/gears.cpp
        ${HHUOS_SRC_DIR}/application/tinygl/info.cpp
        ${HHUOS_SRC_DIR}/application/tinygl/triangle.cpp
        ${HHUOS_SRC_DIR}/application/tinygl/tutorial/lesson1.cpp
        ${HHUOS_SRC_DIR}/application/tinygl/tutorial/lesson2.cpp
        ${HHUOS_SRC_DIR}/application/tinygl/tutorial/lesson3.cpp
        ${HHUOS_SRC_DIR}/application/tinygl/tutorial/lesson4.cpp
        ${HHUOS_SRC_DIR}/application/tinygl/tutorial/lesson5.cpp
        ${HHUOS_SRC_DIR}/application/tinygl/tutorial/lesson6.cpp
        ${HHUOS_SRC_DIR}/application/tinygl/tutorial/lesson7.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} lib.user.runtime lib.user.base lib.user.tinygl)