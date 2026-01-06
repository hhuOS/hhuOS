# Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

project(filesystem)
message(STATUS "Project " ${PROJECT_NAME})

include_directories(${HHUOS_SRC_DIR} ${HHUOS_SRC_DIR}/lib ${HHUOS_SRC_DIR}/lib/libc)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_sources(${PROJECT_NAME} PUBLIC
        ${HHUOS_SRC_DIR}/filesystem/Filesystem.cpp)

# Add subdirectories
add_subdirectory(acpi)
add_subdirectory(fat)
add_subdirectory(iso9660)
add_subdirectory(memory)
add_subdirectory(process)
add_subdirectory(qemu)
add_subdirectory(smbios)
add_subdirectory(tar)
