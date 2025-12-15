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

project(lib.user.pulsar)
message(STATUS "Project " ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)
add_compile_options("-Wpedantic")

add_compile_options(
        -Wno-unused-function
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-parameter>
        $<$<COMPILE_LANGUAGE:C>:-Wno-implicit-fallthrough>
        $<$<COMPILE_LANGUAGE:C>:-Wno-overflow>
)

include_directories(${HHUOS_SRC_DIR} ${HHUOS_SRC_DIR}/lib ${HHUOS_SRC_DIR}/lib/libc)
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib.user.runtime lib.user.base lib.user.graphic lib.user.sound lib.user.math lib.user.time lib.user.tinygl)
target_sources(${PROJECT_NAME} PUBLIC
        ${HHUOS_SRC_DIR}/lib/pulsar/Engine.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/Game.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/Graphics.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/Scene.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/TextScreen.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/2d/Entity.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/2d/Polygon.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/2d/Scene.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/2d/Sprite.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/2d/collider/RectangleCollider.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/2d/component/GravityComponent.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/2d/component/LinearMovementComponent.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/2d/particle/Emitter.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/2d/particle/OnceEmitter.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/2d/particle/Particle.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/3d/Entity.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/3d/Model.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/3d/ObjectFile.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/3d/Orientation.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/3d/Scene.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/3d/Texture.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/audio/AudioBuffer.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/audio/AudioChannel.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/audio/AudioHandle.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/audio/AudioTrack.cpp
        ${HHUOS_SRC_DIR}/lib/pulsar/audio/AudioRunnable.cpp)