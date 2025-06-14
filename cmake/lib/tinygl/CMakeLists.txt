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

project(lib.user.tinygl)
message(STATUS "Project " ${PROJECT_NAME})

include_directories(${HHUOS_SRC_DIR} ${HHUOS_SRC_DIR}/lib/libc ${HHUOS_SRC_DIR}/lib/util)

add_compile_options(
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-function>
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-parameter>
        $<$<COMPILE_LANGUAGE:C>:-Wno-implicit-fallthrough>
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} lib.user.base lib.user.time lib.user.math lib.user.runtime)

target_sources(${PROJECT_NAME} PUBLIC
        ${HHUOS_SRC_DIR}/lib/tinygl/src/accum.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/api.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/arrays.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/clear.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/clip.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/get.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/image_util.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/init.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/light.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/list.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/matrix.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/memory.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/misc.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/msghandling.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/select.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/specbuf.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/texture.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/vertex.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/zbuffer.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/zline.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/zmath.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/zpostprocess.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/zraster.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/ztext.c
        ${HHUOS_SRC_DIR}/lib/tinygl/src/ztriangle.c)