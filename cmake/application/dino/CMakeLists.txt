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

project(dino)
message(STATUS "Project " ${PROJECT_NAME})

add_compile_options(
        -Wno-unused-function
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-parameter>
        $<$<COMPILE_LANGUAGE:C>:-Wno-implicit-fallthrough>
)

include_directories(${HHUOS_SRC_DIR} ${HHUOS_SRC_DIR}/lib ${HHUOS_SRC_DIR}/lib/libc)

set(SOURCE_FILES
        ${HHUOS_SRC_DIR}/application/dino/dino.cpp
        ${HHUOS_SRC_DIR}/application/dino/entity/Block.cpp
        ${HHUOS_SRC_DIR}/application/dino/entity/Coin.cpp
        ${HHUOS_SRC_DIR}/application/dino/entity/EnemyFrog.cpp
        ${HHUOS_SRC_DIR}/application/dino/entity/Fruit.cpp
        ${HHUOS_SRC_DIR}/application/dino/entity/PlayerDino.cpp
        ${HHUOS_SRC_DIR}/application/dino/particle/BloodEmitter.cpp
        ${HHUOS_SRC_DIR}/application/dino/particle/GrassEmitter.cpp
        ${HHUOS_SRC_DIR}/application/dino/GameOverScreen.cpp
        ${HHUOS_SRC_DIR}/application/dino/IntroScreen.cpp
        ${HHUOS_SRC_DIR}/application/dino/Level.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} lib.user.runtime lib.user.base lib.user.pulsar)