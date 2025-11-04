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
        ${HHUOS_SRC_DIR}/application/demo/sprites/DemoSpriteFactory.cpp
        ${HHUOS_SRC_DIR}/application/demo/widget/WidgetApplication.cpp
        ${HHUOS_SRC_DIR}/application/demo/widget/WidgetDemo.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} lib.user.runtime lib.user.base lib.user.pulsar)