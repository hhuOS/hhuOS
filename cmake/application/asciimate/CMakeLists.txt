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

project(asciimate)
message(STATUS "Project " ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)
add_compile_options("-Wpedantic")

make_readme_includable(${HHUOS_SRC_DIR}/application/asciimate)

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/asciimation/"
        COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/asciimation/")

# Do not include the Star Wars asciimation files in redistributable builds (e.g. GitHub Releases).
if ($ENV{HHUOS_REDISTRIBUTABLE_BUILD})
    add_custom_target(asciimation-star-wars DEPENDS
            "${CMAKE_BINARY_DIR}/asciimation/")
else()
    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/asciimation/star_wars.txt"
            COMMAND wget "https://www.asciimation.co.nz/asciimation/SwPlay.jar"
            COMMAND unzip -u "SwPlay.jar" -d "star_wars"
            COMMAND /bin/echo 13,67 | /bin/cat - "star_wars/data/sw1.txt" >> "${CMAKE_BINARY_DIR}/asciimation/star_wars.txt"
            DEPENDS "${CMAKE_BINARY_DIR}/asciimation/")

    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/asciimation/jar_jar.txt"
            COMMAND wget "https://www.asciimation.co.nz/asciimation/jjplay.jar"
            COMMAND unzip -u "jjplay.jar" -d "jar_jar"
            COMMAND /bin/echo 22,81 | /bin/cat - "jar_jar/data/jj1.txt" >> "${CMAKE_BINARY_DIR}/asciimation/jar_jar.txt"
            DEPENDS "${CMAKE_BINARY_DIR}/asciimation/")

    add_custom_target(asciimation-star-wars DEPENDS
            "${CMAKE_BINARY_DIR}/asciimation/star_wars.txt"
            "${CMAKE_BINARY_DIR}/asciimation/jar_jar.txt")
endif()

include_directories(${HHUOS_SRC_DIR} ${HHUOS_SRC_DIR}/lib ${HHUOS_SRC_DIR}/lib/libc)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib.user.runtime lib.user.base lib.user.graphic)
target_sources(${PROJECT_NAME} PUBLIC
        ${HHUOS_SRC_DIR}/application/asciimate/asciimate.cpp)