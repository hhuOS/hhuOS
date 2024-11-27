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

project(clownmdemu)
message(STATUS "Project " ${PROJECT_NAME})

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/megadrive"
        COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/megadrive")

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/megadrive/miniplanets.bin"
        COMMAND wget "'https://github.com/sikthehedgehog/miniplanets/raw/refs/heads/master/Miniplanets%20REMIX%20Ver%20(REV04).bin'" -O "${CMAKE_BINARY_DIR}/megadrive/miniplanets.bin"
        COMMAND /bin/touch -a -m "${CMAKE_BINARY_DIR}/megadrive/miniplanets.bin"
        DEPENDS "${CMAKE_BINARY_DIR}/megadrive")

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/megadrive/witch.bin"
        COMMAND wget "https://github.com/sikthehedgehog/dragon/raw/refs/heads/master/witch.bin" -O "${CMAKE_BINARY_DIR}/megadrive/witch.bin"
        COMMAND /bin/touch -a -m "${CMAKE_BINARY_DIR}/megadrive/witch.bin"
        DEPENDS "${CMAKE_BINARY_DIR}/megadrive")

add_custom_target(megadrive-roms DEPENDS
        "${CMAKE_BINARY_DIR}/megadrive/miniplanets.bin"
        "${CMAKE_BINARY_DIR}/megadrive/witch.bin")

include_directories(${HHUOS_SRC_DIR})

add_compile_options(-Wno-unused-but-set-variable -Wno-parentheses)

# Set source files
set(SOURCE_FILES
        # Application frontend
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu.cpp

        # 68k interpreter
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/clown68000/common/opcode.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/clown68000/interpreter/clown68000.c

        # Emulator
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/bus-common.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/bus-common.h
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/bus-main-m68k.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/bus-main-m68k.h
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/bus-sub-m68k.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/bus-sub-m68k.h
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/bus-z80.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/bus-z80.h
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/clowncommon/clowncommon.h
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/clownmdemu.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/clownmdemu.h
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/controller.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/controller.h
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/fm.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/fm.h
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/fm-channel.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/fm-channel.h
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/fm-operator.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/fm-operator.h
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/fm-phase.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/fm-phase.h
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/io-port.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/io-port.h
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/log.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/log.h
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/pcm.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/pcm.h
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/psg.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/psg.h
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/vdp.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/vdp.h
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/z80.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/z80.h)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} lib.user.runtime lib.user.base)