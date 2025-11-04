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

project(lib.reflection)
message(STATUS "Project " ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 90)

include_directories(${HHUOS_SRC_DIR} ${HHUOS_SRC_DIR}/lib ${HHUOS_SRC_DIR}/lib/libc)
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_sources(${PROJECT_NAME} PUBLIC
        ${HHUOS_SRC_DIR}/lib/util/reflection/InstanceFactory.cpp)

# Kernel space version
project(lib.kernel.reflection)
message(STATUS "Project " ${PROJECT_NAME})
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib.reflection)
target_sources(${PROJECT_NAME} PUBLIC ${HHUOS_SRC_DIR}/lib/kernel.cpp)

# User space version
project(lib.user.reflection)
message(STATUS "Project " ${PROJECT_NAME})
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib.reflection)
target_sources(${PROJECT_NAME} PUBLIC ${HHUOS_SRC_DIR}/lib/user.cpp)
