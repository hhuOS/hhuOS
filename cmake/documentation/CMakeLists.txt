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

cmake_minimum_required(VERSION 3.14)

# Add doxide (if available)
find_program(doxide_path NAMES doxide)
find_program(mkdocs_path NAMES mkdocs)
if (doxide_path)
    project(docs-doxide)
    message(STATUS "Project " ${PROJECT_NAME})

    add_custom_target(${PROJECT_NAME} ALL
            WORKING_DIRECTORY "${HHUOS_ROOT_DIR}/"
            COMMAND /bin/rm -rf "docs/" "site/"
            COMMAND "${doxide_path}" init
            COMMAND "${doxide_path}" build
            COMMAND "${mkdocs_path}" build)
endif()

# Add doxygen (if available)
find_program(doxygen_path NAMES doxygen)
if (doxygen_path)
    project(docs-doxygen)
    message(STATUS "Project " ${PROJECT_NAME})

    add_custom_target(${PROJECT_NAME} ALL
            WORKING_DIRECTORY "${HHUOS_ROOT_DIR}/"
            COMMAND /bin/rm -rf "doxygen/"
            COMMAND "${doxygen_path}" doxygen.conf)
endif()