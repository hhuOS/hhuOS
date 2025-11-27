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

project(litenes)
message(STATUS "Project " ${PROJECT_NAME})

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/roms/nes15.nes" "${CMAKE_BINARY_DIR}/roms/falling.nes"
        COMMAND wget "https://github.com/christopherpow/nes-test-roms/raw/refs/heads/master/nes15-1.0.0/nes15-PAL.nes" -O "${CMAKE_BINARY_DIR}/roms/nes15.nes"
        COMMAND wget "https://github.com/xram64/falling-nes/raw/refs/heads/master/falling.nes" -O "${CMAKE_BINARY_DIR}/roms/falling.nes"
        COMMAND /bin/touch -a -m "${CMAKE_BINARY_DIR}/roms/nes15.nes"
        COMMAND /bin/touch -a -m "${CMAKE_BINARY_DIR}/roms/falling.nes"
        DEPENDS "${CMAKE_BINARY_DIR}/roms")

add_custom_target(nes-roms DEPENDS
        "${CMAKE_BINARY_DIR}/roms/nes15.nes"
        "${CMAKE_BINARY_DIR}/roms/falling.nes")

include_directories(${HHUOS_SRC_DIR}
        ${HHUOS_SRC_DIR}/lib
        ${HHUOS_SRC_DIR}/lib/libc
        ${HHUOS_SRC_DIR}/application/litenes/LiteNES/include)

set(SOURCE_FILES
        ${HHUOS_SRC_DIR}/application/litenes/LiteNES/src/fce/common.c
        ${HHUOS_SRC_DIR}/application/litenes/LiteNES/src/fce/cpu.c
        ${HHUOS_SRC_DIR}/application/litenes/LiteNES/src/fce/cpu-addressing.c
        ${HHUOS_SRC_DIR}/application/litenes/LiteNES/src/fce/fce.c
        ${HHUOS_SRC_DIR}/application/litenes/LiteNES/src/fce/memory.c
        ${HHUOS_SRC_DIR}/application/litenes/LiteNES/src/fce/mmc.c
        ${HHUOS_SRC_DIR}/application/litenes/LiteNES/src/fce/ppu.c
        ${HHUOS_SRC_DIR}/application/litenes/LiteNES/src/fce/psg.c
        ${HHUOS_SRC_DIR}/application/litenes/litenes.cpp)

add_compile_options(
        $<$<COMPILE_LANGUAGE:C>:-fcommon>
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} lib.user.runtime lib.user.base lib.user.libc lib.user.graphic)