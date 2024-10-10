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

project(play)
message(STATUS "Project " ${PROJECT_NAME})

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/wav/"
        COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/wav/")

if ($ENV{HHUOS_REDISTRIBUTABLE_BUILD})
    add_custom_target(wav-files DEPENDS
            "${CMAKE_BINARY_DIR}/wav/")
else()
    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/wav/ukulele.wav"
            COMMAND wget "https://www.bensound.com/bensound-music/bensound-ukulele.mp3" -O "ukulele.mp3"
            COMMAND ffmpeg -i "ukulele.mp3" -acodec pcm_u8 -ar 22050 -ac 1 "${CMAKE_BINARY_DIR}/wav/ukulele.wav"
            DEPENDS "${CMAKE_BINARY_DIR}/wav/")

    add_custom_target(wav-files DEPENDS
            "${CMAKE_BINARY_DIR}/wav/ukulele.wav")
endif()

include_directories(${HHUOS_SRC_DIR})

# Set source files
set(SOURCE_FILES
        ${HHUOS_SRC_DIR}/application/play/play.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} lib.user.runtime lib.user.base lib.user.graphic lib.user.sound)