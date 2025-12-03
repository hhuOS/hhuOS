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

include(ProcessorCount)

# Set version
execute_process(COMMAND git describe --abbrev=0 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE HHUOS_VERSION ERROR_QUIET)
string(STRIP "${HHUOS_VERSION}" HHUOS_VERSION)
if (HHUOS_VERSION STREQUAL "")
    set(HHUOS_VERSION "v0.0.0")
endif ()

# Set codename
execute_process(COMMAND git show ${HHUOS_VERSION} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND sed -n 5p WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND cut -d " " -f 3 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE HHUOS_CODENAME ERROR_QUIET)
string(STRIP "${HHUOS_CODENAME}" HHUOS_CODENAME)
if (HHUOS_CODENAME STREQUAL "")
    set(HHUOS_CODENAME "Unknown")
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
message("hhuOS ${HHUOS_VERSION} (${HHUOS_CODENAME}) - git ${HHUOS_GIT_REV}, build date: ${HHUOS_BUILD_DATE}")

# Set source directory variables
set(HHUOS_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
set(HHUOS_SRC_DIR "${HHUOS_ROOT_DIR}/src")
set(HHUOS_TOOL_DIR "${HHUOS_ROOT_DIR}/tools")

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

add_compile_options(
        # General compiler flags
        -m32
        -march=i386
        -mfpmath=387
        -mno-mmx
        -mno-sse
        -mno-avx
        -fno-stack-protector
        -fno-pic
        -no-pie
        -ffreestanding
        -Wall
        -Wextra
        -Werror
        $<$<VERSION_GREATER_EQUAL:${CMAKE_C_COMPILER_VERSION},9>:-mmanual-endbr>

        # C++ compiler flags
        $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<VERSION_GREATER_EQUAL:${CMAKE_C_COMPILER_VERSION},15>>:-fno-assume-sane-operators-new-delete>

        # Macro definitions
        -U__linux__
        -DHHUOS_VERSION=${HHUOS_VERSION}
        -DHHUOS_CODENAME=${HHUOS_CODENAME}
        -DHHUOS_GIT_REV=${HHUOS_GIT_REV}
        -DHHUOS_GIT_BRANCH=${HHUOS_GIT_BRANCH}
        -DHHUOS_BUILD_DATE=${HHUOS_BUILD_DATE}
        -DHHUOS_BUILD_TYPE=${CMAKE_BUILD_TYPE}
)

add_link_options(
        -nostartfiles
        -nodefaultlibs
        -nolibc
        -nostdlib
        $<$<VERSION_GREATER_EQUAL:${CMAKE_C_COMPILER_VERSION},13>:-nostdlib++>
        -no-pie
        -Wl,--build-id=none
        -Wl,-m,elf_i386
        -T $<IF:$<STREQUAL:$<TARGET_PROPERTY:NAME>,system>,${HHUOS_SRC_DIR}/link.ld,${HHUOS_SRC_DIR}/application/link.ld>
)

# Add include-what-you-use command (if available)
find_package(Python3)
find_program(iwyu_tool_path NAMES iwyu_tool.py iwyu-tool)
find_program(iwyu_fix_includes_path NAMES fix_includes.py iwyu-fix-includes)
if (Python3_Interpreter_FOUND AND iwyu_tool_path AND iwyu_fix_includes_path)
    ProcessorCount(ncpus)
    add_custom_target(include-what-you-use
            COMMAND "${Python3_EXECUTABLE}" "${iwyu_tool_path}" -j "${ncpus}" -p "${CMAKE_BINARY_DIR}" |
                    "${Python3_EXECUTABLE}" "${iwyu_fix_includes_path}" --nocomments --no_fwd_decls --noupdate_comments --blank_lines --nosafe_headers --ignore_re "(.*/ff/source.*)|(.*/tinygl.*)|(.*/portablegl.*)|(.*/libc.*)|(.*/doomgeneric/doomgeneric.*)|(.*/quakegeneric/source.*)|(.*/Peanut-GB.*)|(.*/clownmdemu/clownmdemu.*)|(.*/classicube/ClassiCube.*)|(.*/litenes/LiteNES.*)" -p "${HHUOS_SRC_DIR}"
            VERBATIM)
endif()

# Add cloc command (if available)
find_program(cloc_path NAMES cloc)
if (cloc_path)
    add_custom_target(cloc COMMAND "${cloc_path}" "${HHUOS_SRC_DIR}" --exclude-dir=ff,tinygl,portablegl,doomgeneric,quakegeneric,Peanut-GB,clownmdemu,ClassiCube,LiteNES VERBATIM)
endif()

# Add subdirectories
add_subdirectory(application)
add_subdirectory(device)
add_subdirectory(filesystem)
add_subdirectory(kernel)
add_subdirectory(lib)
add_subdirectory(system)

add_subdirectory(grub)
add_subdirectory(limine)
add_subdirectory(towboot)

add_subdirectory(books)
add_subdirectory(floppy0)
add_subdirectory(hdd0)

add_subdirectory(documentation)