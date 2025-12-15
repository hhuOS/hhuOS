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

project(lib.user.lunar)
message(STATUS "Project " ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)
add_compile_options("-Wpedantic")

include_directories(${HHUOS_SRC_DIR} ${HHUOS_SRC_DIR}/lib ${HHUOS_SRC_DIR}/lib/libc)
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib.user.runtime lib.user.base lib.user.graphic)
target_sources(${PROJECT_NAME} PUBLIC
        ${HHUOS_SRC_DIR}/lib/lunar/BorderLayout.cpp
        ${HHUOS_SRC_DIR}/lib/lunar/Button.cpp
        ${HHUOS_SRC_DIR}/lib/lunar/CheckBox.cpp
        ${HHUOS_SRC_DIR}/lib/lunar/Container.cpp
        ${HHUOS_SRC_DIR}/lib/lunar/FreeLayout.cpp
        ${HHUOS_SRC_DIR}/lib/lunar/GridLayout.cpp
        ${HHUOS_SRC_DIR}/lib/lunar/HorizontalLayout.cpp
        ${HHUOS_SRC_DIR}/lib/lunar/InputField.cpp
        ${HHUOS_SRC_DIR}/lib/lunar/Label.cpp
        ${HHUOS_SRC_DIR}/lib/lunar/RadioButton.cpp
        ${HHUOS_SRC_DIR}/lib/lunar/RadioButtonGroup.cpp
        ${HHUOS_SRC_DIR}/lib/lunar/Theme.cpp
        ${HHUOS_SRC_DIR}/lib/lunar/VerticalLayout.cpp
        ${HHUOS_SRC_DIR}/lib/lunar/Widget.cpp)