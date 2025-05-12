# Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

project(demo)
message(STATUS "Project " ${PROJECT_NAME})

add_compile_options(
        -Wno-unused-function
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-parameter>
        $<$<COMPILE_LANGUAGE:C>:-Wno-implicit-fallthrough>
)

include_directories(${HHUOS_SRC_DIR} ${HHUOS_SRC_DIR}/lib/libc ${HHUOS_SRC_DIR}/lib/util)

set(SOURCE_FILES
        ${HHUOS_SRC_DIR}/application/demo/demo.cpp
        ${HHUOS_SRC_DIR}/application/demo/ant/Ant.cpp
        ${HHUOS_SRC_DIR}/application/demo/color/AnsiColorDemo.cpp
        ${HHUOS_SRC_DIR}/application/demo/fonts/FontDemo.cpp
        ${HHUOS_SRC_DIR}/application/demo/mouse/Logo.cpp
        ${HHUOS_SRC_DIR}/application/demo/mouse/MouseCursor.cpp
        ${HHUOS_SRC_DIR}/application/demo/mouse/MouseDemo.cpp
        ${HHUOS_SRC_DIR}/application/demo/opengl/Cuboid.cpp
        ${HHUOS_SRC_DIR}/application/demo/opengl/DemoModel.cpp
        ${HHUOS_SRC_DIR}/application/demo/opengl/Rectangle.cpp
        ${HHUOS_SRC_DIR}/application/demo/opengl/OpenGlDemo.cpp
        ${HHUOS_SRC_DIR}/application/demo/particles/Dino.cpp
        ${HHUOS_SRC_DIR}/application/demo/particles/DropletEmitter.cpp
        ${HHUOS_SRC_DIR}/application/demo/particles/Ground.cpp
        ${HHUOS_SRC_DIR}/application/demo/particles/ParticleDemo.cpp
        ${HHUOS_SRC_DIR}/application/demo/particles/RainEmitter.cpp
        ${HHUOS_SRC_DIR}/application/demo/polygons/DemoPolygon.cpp
        ${HHUOS_SRC_DIR}/application/demo/polygons/DemoPolygonFactory.cpp
        ${HHUOS_SRC_DIR}/application/demo/polygons/PolygonDemo.cpp
        ${HHUOS_SRC_DIR}/application/demo/sprites/SpriteDemo.cpp
        ${HHUOS_SRC_DIR}/application/demo/sprites/DemoSprite.cpp
        ${HHUOS_SRC_DIR}/application/demo/sprites/DemoSpriteFactory.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} lib.user.runtime lib.user.base lib.user.game)