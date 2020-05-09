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

project(music)
message(STATUS "Project " ${PROJECT_NAME})

set(BEEPC_DIR ${HHUOS_TOOL_DIR}/beepc)
set(BEEP_SOURCE_DIR ${HHUOS_ROOT_DIR}/music/beep)
set(BEEP_OUTPUT_DIR ${HHUOS_ROOT_DIR}/initrd/music/beep)

set(CMAKE_CXX_FLAGS "-std=c++17")

set(SOURCE_FILES
        ${BEEPC_DIR}/beepc.cpp)

add_executable(beepc ${SOURCE_FILES})
target_link_libraries(beepc stdc++fs)

set_target_properties(beepc PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${HHUOS_TOOL_DIR}/beepc/)

add_custom_command(OUTPUT "${BEEP_OUTPUT_DIR}/aerodynamic.beep"
        COMMAND ${HHUOS_ROOT_DIR}/tool/beepc/beepc "${BEEP_SOURCE_DIR}/aerodynamic.beepsrc" "${BEEP_OUTPUT_DIR}/aerodynamic.beep")
add_custom_command(OUTPUT "${BEEP_OUTPUT_DIR}/error.beep"
        COMMAND ${HHUOS_ROOT_DIR}/tool/beepc/beepc "${BEEP_SOURCE_DIR}/error.beepsrc" "${BEEP_OUTPUT_DIR}/error.beep")
add_custom_command(OUTPUT "${BEEP_OUTPUT_DIR}/mario.beep"
        COMMAND ${HHUOS_ROOT_DIR}/tool/beepc/beepc "${BEEP_SOURCE_DIR}/mario.beepsrc" "${BEEP_OUTPUT_DIR}/mario.beep")
add_custom_command(OUTPUT "${BEEP_OUTPUT_DIR}/monkey.beep"
        COMMAND ${HHUOS_ROOT_DIR}/tool/beepc/beepc "${BEEP_SOURCE_DIR}/monkey.beepsrc" "${BEEP_OUTPUT_DIR}/monkey.beep")
add_custom_command(OUTPUT "${BEEP_OUTPUT_DIR}/startup.beep"
        COMMAND ${HHUOS_ROOT_DIR}/tool/beepc/beepc "${BEEP_SOURCE_DIR}/startup.beepsrc" "${BEEP_OUTPUT_DIR}/startup.beep")
add_custom_command(OUTPUT "${BEEP_OUTPUT_DIR}/starwars.beep"
        COMMAND ${HHUOS_ROOT_DIR}/tool/beepc/beepc "${BEEP_SOURCE_DIR}/starwars.beepsrc" "${BEEP_OUTPUT_DIR}/starwars.beep")
add_custom_command(OUTPUT "${BEEP_OUTPUT_DIR}/tetris.beep"
        COMMAND ${HHUOS_ROOT_DIR}/tool/beepc/beepc "${BEEP_SOURCE_DIR}/tetris.beepsrc" "${BEEP_OUTPUT_DIR}/tetris.beep")

add_custom_target(aerodynamic DEPENDS beepc ${BEEP_OUTPUT_DIR}/aerodynamic.beep)
add_custom_target(error DEPENDS beepc ${BEEP_OUTPUT_DIR}/error.beep)
add_custom_target(mario DEPENDS beepc ${BEEP_OUTPUT_DIR}/mario.beep)
add_custom_target(monkey DEPENDS beepc ${BEEP_OUTPUT_DIR}/monkey.beep)
add_custom_target(startup DEPENDS beepc ${BEEP_OUTPUT_DIR}/startup.beep)
add_custom_target(starwars DEPENDS beepc ${BEEP_OUTPUT_DIR}/starwars.beep)
add_custom_target(tetris DEPENDS beepc ${BEEP_OUTPUT_DIR}/tetris.beep)

add_custom_target(beep DEPENDS aerodynamic error mario startup starwars tetris)