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

cmake_minimum_required(VERSION 3.5)

project(application)
message(STATUS "Project " ${PROJECT_NAME})

include_directories(${HHUOS_SRC_DIR})

set(SOURCE_FILES
        ${HHUOS_SRC_DIR}/application/Application.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

# Add subdirectories
add_subdirectory(ant)
add_subdirectory(bug)
add_subdirectory(loop)
add_subdirectory(mandelbrot)
add_subdirectory(memory)
add_subdirectory(mouse)
add_subdirectory(shell)

target_link_libraries(${PROJECT_NAME} device filesystem kernel lib)
