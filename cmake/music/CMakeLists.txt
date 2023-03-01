# Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

project(music)
message(STATUS "Project " ${PROJECT_NAME})

if ($ENV{HHUOS_REDISTRIBUTABLE_BUILD})
    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/"
            COMMAND COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/beep/")

    add_custom_target(${PROJECT_NAME} DEPENDS
            "${CMAKE_BINARY_DIR}/beep/")
else()
    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/angel.beep"
            COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/beep/"
            COMMAND python3 "${HHUOS_TOOL_DIR}/beep.py" -i https://pianoletternotes.blogspot.com/2020/01/cruel-angels-thesis-neon-genesis.html > "${CMAKE_BINARY_DIR}/beep/angel.beep")

    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/castlevania.beep"
            COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/beep/"
            COMMAND python3 "${HHUOS_TOOL_DIR}/beep.py" -i https://pianoletternotes.blogspot.com/2020/10/vampire-killer-castlevania.html > "${CMAKE_BINARY_DIR}/beep/castlevania.beep")

    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/dance_monkey.beep"
            COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/beep/"
            COMMAND python3 "${HHUOS_TOOL_DIR}/beep.py" -i https://pianoletternotes.blogspot.com/2020/04/dance-monkey-by-tones-and-i-hard-version.html > "${CMAKE_BINARY_DIR}/beep/dance_monkey.beep")

    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/deja_vu.beep"
            COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/beep/"
            COMMAND python3 "${HHUOS_TOOL_DIR}/beep.py" -i https://pianoletternotes.blogspot.com/2019/02/deja-vu-initial-d.html -t 94 > "${CMAKE_BINARY_DIR}/beep/deja_vu.beep")

    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/ghostbusters.beep"
            COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/beep/"
            COMMAND python3 "${HHUOS_TOOL_DIR}/beep.py" -i https://pianoletternotes.blogspot.com/2019/10/ghostbusters-theme-hard-version.html > "${CMAKE_BINARY_DIR}/beep/ghostbusters.beep")

    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/harry_potter.beep"
            COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/beep/"
            COMMAND python3 "${HHUOS_TOOL_DIR}/beep.py" -i https://pianoletternotes.blogspot.com/2019/07/nimbus-2000-harry-potter.html -t 94 > "${CMAKE_BINARY_DIR}/beep/harry_potter.beep")

    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/mario.beep"
            COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/beep/"
            COMMAND python3 "${HHUOS_TOOL_DIR}/beep.py" -i https://pianoletternotes.blogspot.com/2022/06/overworld-theme-new-super-mario-bros-ds.html -t 94 > "${CMAKE_BINARY_DIR}/beep/mario.beep")

    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/sonne.beep"
            COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/beep/"
            COMMAND python3 "${HHUOS_TOOL_DIR}/beep.py" -i https://pianoletternotes.blogspot.com/2022/01/sonne-by-rammstein.html > "${CMAKE_BINARY_DIR}/beep/sonne.beep")

    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/star_wars.beep"
            COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/beep/"
            COMMAND python3 "${HHUOS_TOOL_DIR}/beep.py" -i https://pianoletternotes.blogspot.com/2015/11/star-wars-main-theme.html > "${CMAKE_BINARY_DIR}/beep/star_wars.beep")

    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/tetris.beep"
            COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/beep/"
            COMMAND python3 "${HHUOS_TOOL_DIR}/beep.py" -i https://pianoletternotes.blogspot.com/2019/10/tetris-theme-hard-version.html > "${CMAKE_BINARY_DIR}/beep/tetris.beep")

    add_custom_target(${PROJECT_NAME} DEPENDS
            "${CMAKE_BINARY_DIR}/beep/angel.beep"
            "${CMAKE_BINARY_DIR}/beep/castlevania.beep"
            "${CMAKE_BINARY_DIR}/beep/dance_monkey.beep"
            "${CMAKE_BINARY_DIR}/beep/deja_vu.beep"
            "${CMAKE_BINARY_DIR}/beep/ghostbusters.beep"
            "${CMAKE_BINARY_DIR}/beep/harry_potter.beep"
            "${CMAKE_BINARY_DIR}/beep/mario.beep"
            "${CMAKE_BINARY_DIR}/beep/sonne.beep"
            "${CMAKE_BINARY_DIR}/beep/star_wars.beep"
            "${CMAKE_BINARY_DIR}/beep/tetris.beep")
endif()