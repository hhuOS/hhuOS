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

project(lib.user.libc)
message(STATUS "Project " ${PROJECT_NAME})

include_directories(${HHUOS_SRC_DIR})

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} lib.user.base lib.user.time lib.user.math lib.user.runtime)

target_sources(${PROJECT_NAME} PUBLIC
		${HHUOS_SRC_DIR}/lib/libc/errno.cpp
		${HHUOS_SRC_DIR}/lib/libc/ctype.cpp
        ${HHUOS_SRC_DIR}/lib/libc/stdlib.cpp
		${HHUOS_SRC_DIR}/lib/libc/string.cpp	
		${HHUOS_SRC_DIR}/lib/libc/math.cpp
		${HHUOS_SRC_DIR}/lib/libc/locale.cpp
		${HHUOS_SRC_DIR}/lib/libc/setjmp.asm
		${HHUOS_SRC_DIR}/lib/libc/time.cpp
		${HHUOS_SRC_DIR}/lib/libc/stdio.cpp
		${HHUOS_SRC_DIR}/lib/libc/stdio_printf.cpp
		${HHUOS_SRC_DIR}/lib/libc/stdio_scanf.cpp
		${HHUOS_SRC_DIR}/lib/libc/stdarg.cpp
		${HHUOS_SRC_DIR}/lib/libc/sys/stat.cpp)