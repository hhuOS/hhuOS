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

project(lib.io)
message(STATUS "Project " ${PROJECT_NAME})

include_directories(${HHUOS_SRC_DIR})

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_sources(${PROJECT_NAME} PUBLIC
        ${HHUOS_SRC_DIR}/lib/util/io/file/File.cpp
        ${HHUOS_SRC_DIR}/lib/util/io/file/elf/File.cpp
        ${HHUOS_SRC_DIR}/lib/util/io/file/tar/Archive.cpp
        ${HHUOS_SRC_DIR}/lib/util/io/key/Key.cpp
        ${HHUOS_SRC_DIR}/lib/util/io/key/KeyDecoder.cpp
        ${HHUOS_SRC_DIR}/lib/util/io/key/KeyboardLayout.cpp
        ${HHUOS_SRC_DIR}/lib/util/io/key/MouseDecoder.cpp
        ${HHUOS_SRC_DIR}/lib/util/io/stream/BufferedInputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/io/stream/BufferedOutputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/io/stream/ByteArrayOutputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/io/stream/ByteArrayInputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/io/stream/FileInputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/io/stream/FileOutputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/io/stream/FilterInputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/io/stream/FilterOutputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/io/stream/InputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/io/stream/OutputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/io/stream/PipedInputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/io/stream/PipedOutputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/io/stream/PrintStream.cpp
		${HHUOS_SRC_DIR}/lib/util/io/stream/PrintStream_printf.cpp
		${HHUOS_SRC_DIR}/lib/util/io/stream/PrintStream_time.cpp
		${HHUOS_SRC_DIR}/lib/util/io/stream/ScanStream.cpp
		${HHUOS_SRC_DIR}/lib/util/io/stream/ScanStream_scanf.cpp
		${HHUOS_SRC_DIR}/lib/util/io/stream/FileStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/io/stream/QueueInputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/io/stream/QueueOutputStream.cpp)

# Kernel space version
project(lib.kernel.io)
message(STATUS "Project " ${PROJECT_NAME})
include_directories(${HHUOS_SRC_DIR})
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib.io)
target_sources(${PROJECT_NAME} PUBLIC ${HHUOS_SRC_DIR}/lib/kernel.cpp)

# User space version
project(lib.user.io)
message(STATUS "Project " ${PROJECT_NAME})
include_directories(${HHUOS_SRC_DIR})
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib.io)
target_sources(${PROJECT_NAME} PUBLIC ${HHUOS_SRC_DIR}/lib/user.cpp)