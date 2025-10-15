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

project(lib.game)
message(STATUS "Project " ${PROJECT_NAME})

include_directories(${HHUOS_SRC_DIR} ${HHUOS_SRC_DIR}/lib/libc ${HHUOS_SRC_DIR}/lib/util)

add_compile_options(
        -Wno-unused-function
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-parameter>
        $<$<COMPILE_LANGUAGE:C>:-Wno-implicit-fallthrough>
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} lib.graphic lib.sound lib.user.tinygl lib.user.libc)

target_sources(${PROJECT_NAME} PUBLIC
        ${HHUOS_SRC_DIR}/lib/util/game/Camera.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/CancelableEvent.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/Collider.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/Engine.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/Entity.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/Game.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/GameManager.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/Graphics.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/ResourceManager.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/Scene.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/2d/Entity.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/2d/Polygon.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/2d/Scene.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/2d/Sprite.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/2d/SpriteAnimation.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/2d/collider/PolygonCollider.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/2d/collider/RectangleCollider.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/2d/component/Component.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/2d/component/GravityComponent.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/2d/component/LinearMovementComponent.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/2d/event/CollisionEvent.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/2d/event/TranslationEvent.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/2d/particle/Emitter.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/2d/particle/Particle.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/2d/particle/SingleTimeEmitter.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/3d/Entity.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/3d/Light.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/3d/Model.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/3d/ObjectFile.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/3d/Orientation.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/3d/Scene.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/3d/Texture.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/3d/Util.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/3d/collider/SphereCollider.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/3d/event/CollisionEvent.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/audio/AudioBuffer.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/audio/AudioChannel.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/audio/AudioHandle.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/audio/AudioTrack.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/audio/AudioRunnable.cpp)

# Kernel space version
project(lib.kernel.game)
message(STATUS "Project " ${PROJECT_NAME})
include_directories(${HHUOS_SRC_DIR} ${HHUOS_SRC_DIR}/lib/libc ${HHUOS_SRC_DIR}/lib/util)
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib.game)
target_sources(${PROJECT_NAME} PUBLIC ${HHUOS_SRC_DIR}/lib/kernel.cpp)

# User space version
project(lib.user.game)
message(STATUS "Project " ${PROJECT_NAME})
include_directories(${HHUOS_SRC_DIR} ${HHUOS_SRC_DIR}/lib/libc ${HHUOS_SRC_DIR}/lib/util)
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib.game)
target_sources(${PROJECT_NAME} PUBLIC ${HHUOS_SRC_DIR}/lib/user.cpp)