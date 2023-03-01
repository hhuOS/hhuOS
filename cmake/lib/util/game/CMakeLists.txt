# Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

project(lib.game)
message(STATUS "Project " ${PROJECT_NAME})

include_directories(${HHUOS_SRC_DIR})

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} lib.graphic)

target_sources(${PROJECT_NAME} PUBLIC
        ${HHUOS_SRC_DIR}/lib/util/game/entity/collider/Collider.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/entity/collider/RectangleCollider.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/entity/component/Component.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/entity/component/GravityComponent.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/entity/component/LinearMovementComponent.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/entity/event/Event.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/entity/event/CancelableEvent.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/entity/event/CollisionEvent.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/entity/event/TranslationEvent.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/entity/Entity.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/Camera.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/Engine.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/Game.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/GameManager.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/Graphics2D.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/Polygon.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/ResourceManager.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/Scene.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/Sprite.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/SpriteAnimation.cpp
        ${HHUOS_SRC_DIR}/lib/util/game/Text.cpp)

# Kernel space version
project(lib.kernel.game)
message(STATUS "Project " ${PROJECT_NAME})
include_directories(${HHUOS_SRC_DIR})
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib.game)
target_sources(${PROJECT_NAME} PUBLIC ${HHUOS_SRC_DIR}/lib/kernel.cpp)

# User space version
project(lib.user.game)
message(STATUS "Project " ${PROJECT_NAME})
include_directories(${HHUOS_SRC_DIR})
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib.game)
target_sources(${PROJECT_NAME} PUBLIC ${HHUOS_SRC_DIR}/lib/user.cpp)