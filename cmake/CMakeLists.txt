# Copyright (C) 2019 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
# Heinrich-Heine University
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

cmake_minimum_required(VERSION 3.7)

# Set version
execute_process(COMMAND git describe --tags --abbrev=0 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE HHUOS_VERSION ERROR_QUIET)
string(STRIP "${HHUOS_VERSION}" HHUOS_VERSION)
if (HHUOS_VERSION STREQUAL "")
    set(HHUOS_VERSION "v0.0.0")
endif ()

# Set git revision
execute_process(COMMAND git log -1 --format=%h --date=short HEAD WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE HHUOS_GIT_REV ERROR_QUIET)
string(STRIP "${HHUOS_GIT_REV}" HHUOS_GIT_REV)
if (HHUOS_GIT_REV STREQUAL "")
    set(HHUOS_GIT_REV "not available")
endif ()

# Set git branch
execute_process(COMMAND git rev-parse --abbrev-ref HEAD WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE HHUOS_GIT_BRANCH ERROR_QUIET)
string(STRIP "${HHUOS_GIT_BRANCH}" HHUOS_GIT_BRANCH)
if (HHUOS_GIT_BRANCH STREQUAL "")
    set(HHUOS_GIT_BRANCH "unknown")
endif ()

# Set build date
string(TIMESTAMP HHUOS_BUILD_DATE "%Y-%m-%d %H:%M:%S")

# Print build information
message("hhuOS ${HHUOS_VERSION} - git ${HHUOS_GIT_REV}, build date: ${HHUOS_BUILD_DATE}")

# Set preprocessor defines
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}\
    -DHHUOS_VERSION='${HHUOS_VERSION}'\
    -DHHUOS_GIT_REV='${HHUOS_GIT_REV}'\
    -DHHUOS_GIT_BRANCH='${HHUOS_GIT_BRANCH}'\
    -DHHUOS_BUILD_DATE='${HHUOS_BUILD_DATE}'")

# Set source directory variables
set(HHUOS_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
set(HHUOS_SRC_DIR "${HHUOS_ROOT_DIR}/src")
set(HHUOS_TOOL_DIR "${HHUOS_ROOT_DIR}/tool")

# Set output directory variables
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/static)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/shared)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Force out of source build
message(STATUS "Force out of source build check...")
string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" insource)
if (insource)
    message(FATAL_ERROR "Please do not build in your source dir. Use a dedicated build folder!")
endif ()

# Set compiler-flags
ENABLE_LANGUAGE(ASM_NASM)
set(CMAKE_ASM_NASM_OBJECT_FORMAT "elf32")
set(CMAKE_ASM_NASM_INCLUDES "${HHUOS_SRC_DIR}/")
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> -I${CMAKE_ASM_NASM_INCLUDES} -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -g -Wall -Wno-write-strings -fno-stack-protector -nostdlib -fno-pic -no-pie -ffreestanding -mno-sse -march=i486")
if(CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 9)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mmanual-endbr")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS} -Wl,--build-id=none -Wno-non-virtual-dtor -fno-threadsafe-statics -fno-use-cxa-atexit -fno-rtti -fno-exceptions -std=c++17 -T ${HHUOS_SRC_DIR}/link.ld")

# Add subdirectories
add_subdirectory(application)
add_subdirectory(device)
add_subdirectory(filesystem)
add_subdirectory(kernel)
add_subdirectory(lib)
add_subdirectory(system)
add_subdirectory(iso)

add_subdirectory(initrd)
add_subdirectory(hdd)

add_subdirectory(beep)
