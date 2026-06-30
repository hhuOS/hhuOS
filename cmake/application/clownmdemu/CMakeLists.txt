# Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

project(clownmdemu)
message(STATUS "Project " ${PROJECT_NAME})

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/roms/miniplanets.bin"
        COMMAND wget "'https://github.com/sikthehedgehog/miniplanets/raw/refs/heads/master/Miniplanets%20REMIX%20Ver%20(REV04).bin'" -O "${CMAKE_BINARY_DIR}/roms/miniplanets.bin"
        COMMAND /bin/touch -a -m "${CMAKE_BINARY_DIR}/roms/miniplanets.bin"
        DEPENDS "${CMAKE_BINARY_DIR}/roms")

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/roms/witch.bin"
        COMMAND wget "https://github.com/sikthehedgehog/dragon/raw/refs/heads/master/witch.bin" -O "${CMAKE_BINARY_DIR}/roms/witch.bin"
        COMMAND /bin/touch -a -m "${CMAKE_BINARY_DIR}/roms/witch.bin"
        DEPENDS "${CMAKE_BINARY_DIR}/roms")

add_custom_target(megadrive-roms DEPENDS
        "${CMAKE_BINARY_DIR}/roms/miniplanets.bin"
        "${CMAKE_BINARY_DIR}/roms/witch.bin")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)

make_readme_includable(${HHUOS_SRC_DIR}/application/clownmdemu)

add_compile_options(
        -Wpedantic
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-but-set-variable>
        $<$<COMPILE_LANGUAGE:C>:-Wno-implicit-fallthrough>
        $<$<COMPILE_LANGUAGE:C>:-Wno-parentheses>
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-parameter>
        $<$<COMPILE_LANGUAGE:C>:-Wno-maybe-uninitialized>
        $<$<COMPILE_LANGUAGE:C>:-O0> # ClownMDEmu does not work with compiler optimizations
)

include_directories(${HHUOS_SRC_DIR} ${HHUOS_SRC_DIR}/lib ${HHUOS_SRC_DIR}/lib/libc)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib.user.runtime lib.user.base lib.user.graphic)
target_sources(${PROJECT_NAME} PUBLIC
        # Application frontend
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu.cpp

        # 68k interpreter
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/libraries/clown68000/source/interpreter/unity.c

        # Z80 interpreter
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/libraries/clownz80/source/common.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/libraries/clownz80/source/interpreter.c

        # Emulator
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/bus-common.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/bus-main-m68k.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/bus-sub-m68k.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/bus-z80.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/cdc.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/cdda.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/clownmdemu.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/controller.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/controller-manager.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/controller-multitap-ea.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/controller-multitap-sega.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/fm.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/fm-channel.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/fm-lfo.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/fm-operator.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/fm-phase.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/io-port.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/log.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/low-pass-filter.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/pcm.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/psg.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/sync.c
        ${HHUOS_SRC_DIR}/application/clownmdemu/clownmdemu/source/vdp.c)