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

project(music)
message(STATUS "Project " ${PROJECT_NAME})

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/"
        COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/beep/")

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/wav/"
        COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/wav/")

if ($ENV{HHUOS_REDISTRIBUTABLE_BUILD})
    add_custom_target(${PROJECT_NAME} DEPENDS
            "${CMAKE_BINARY_DIR}/beep/"
            "${CMAKE_BINARY_DIR}/wav/")
else()
    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/wav/ukulele.wav"
            COMMAND wget "https://www.bensound.com/bensound-music/bensound-ukulele.mp3" -O "ukulele.mp3"
            COMMAND ffmpeg -i "ukulele.mp3" -acodec pcm_u8 -ar 22050 -ac 1 "${CMAKE_BINARY_DIR}/wav/ukulele.wav"
            DEPENDS "${CMAKE_BINARY_DIR}/wav/")

    find_package(Python3)
    if (Python3_Interpreter_FOUND)
        add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/angel.beep"
                COMMAND python3 "${HHUOS_TOOL_DIR}/beep.py" -i "https://pianoletternotes.blogspot.com/2020/01/cruel-angels-thesis-neon-genesis.html" > "${CMAKE_BINARY_DIR}/beep/angel.beep"
                DEPENDS "${CMAKE_BINARY_DIR}/beep/")

        add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/castlevania.beep"
                COMMAND python3 "${HHUOS_TOOL_DIR}/beep.py" -i "https://pianoletternotes.blogspot.com/2020/10/vampire-killer-castlevania.html" > "${CMAKE_BINARY_DIR}/beep/castlevania.beep"
                DEPENDS "${CMAKE_BINARY_DIR}/beep/")

        add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/dance_monkey.beep"
                COMMAND python3 "${HHUOS_TOOL_DIR}/beep.py" -i "https://pianoletternotes.blogspot.com/2020/04/dance-monkey-by-tones-and-i-hard-version.html" > "${CMAKE_BINARY_DIR}/beep/dance_monkey.beep"
                DEPENDS "${CMAKE_BINARY_DIR}/beep/")

        add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/deja_vu.beep"
                COMMAND python3 "${HHUOS_TOOL_DIR}/beep.py" -i "https://pianoletternotes.blogspot.com/2019/02/deja-vu-initial-d.html" -t 94 > "${CMAKE_BINARY_DIR}/beep/deja_vu.beep"
                DEPENDS "${CMAKE_BINARY_DIR}/beep/")

        add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/doom.beep"
                COMMAND python3 "${HHUOS_TOOL_DIR}/beep.py" -i "https://pianoletternotes.blogspot.com/2022/07/at-dooms-gate-e1m1-doom.html" -t 65 > "${CMAKE_BINARY_DIR}/beep/doom.beep"
                DEPENDS "${CMAKE_BINARY_DIR}/beep/")

        add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/ghostbusters.beep"
                COMMAND python3 "${HHUOS_TOOL_DIR}/beep.py" -i "https://pianoletternotes.blogspot.com/2019/10/ghostbusters-theme-hard-version.html" > "${CMAKE_BINARY_DIR}/beep/ghostbusters.beep"
                DEPENDS "${CMAKE_BINARY_DIR}/beep/")

        add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/harry_potter.beep"
                COMMAND python3 "${HHUOS_TOOL_DIR}/beep.py" -i "https://pianoletternotes.blogspot.com/2019/07/nimbus-2000-harry-potter.html" -t 94 > "${CMAKE_BINARY_DIR}/beep/harry_potter.beep"
                DEPENDS "${CMAKE_BINARY_DIR}/beep/")

        add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/mario.beep"
                COMMAND python3 "${HHUOS_TOOL_DIR}/beep.py" -i "https://pianoletternotes.blogspot.com/2022/06/overworld-theme-new-super-mario-bros-ds.html" -t 94 > "${CMAKE_BINARY_DIR}/beep/mario.beep"
                DEPENDS "${CMAKE_BINARY_DIR}/beep/")

        add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/sonne.beep"
                COMMAND python3 "${HHUOS_TOOL_DIR}/beep.py" -i "https://pianoletternotes.blogspot.com/2022/01/sonne-by-rammstein.html" > "${CMAKE_BINARY_DIR}/beep/sonne.beep"
                DEPENDS "${CMAKE_BINARY_DIR}/beep/")

        add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/star_wars.beep"
                COMMAND python3 "${HHUOS_TOOL_DIR}/beep.py" -i "https://pianoletternotes.blogspot.com/2015/11/star-wars-main-theme.html" > "${CMAKE_BINARY_DIR}/beep/star_wars.beep"
                DEPENDS "${CMAKE_BINARY_DIR}/beep/")

        add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/beep/tetris.beep"
                COMMAND python3 "${HHUOS_TOOL_DIR}/beep.py" -i "https://pianoletternotes.blogspot.com/2019/10/tetris-theme-hard-version.html" > "${CMAKE_BINARY_DIR}/beep/tetris.beep"
                DEPENDS "${CMAKE_BINARY_DIR}/beep/")

        add_custom_target(${PROJECT_NAME} DEPENDS
                "${CMAKE_BINARY_DIR}/wav/ukulele.wav"
                "${CMAKE_BINARY_DIR}/beep/angel.beep"
                "${CMAKE_BINARY_DIR}/beep/castlevania.beep"
                "${CMAKE_BINARY_DIR}/beep/dance_monkey.beep"
                "${CMAKE_BINARY_DIR}/beep/deja_vu.beep"
                "${CMAKE_BINARY_DIR}/beep/doom.beep"
                "${CMAKE_BINARY_DIR}/beep/ghostbusters.beep"
                "${CMAKE_BINARY_DIR}/beep/harry_potter.beep"
                "${CMAKE_BINARY_DIR}/beep/mario.beep"
                "${CMAKE_BINARY_DIR}/beep/sonne.beep"
                "${CMAKE_BINARY_DIR}/beep/star_wars.beep"
                "${CMAKE_BINARY_DIR}/beep/tetris.beep")
    else()
        add_custom_target(${PROJECT_NAME} DEPENDS
                "${CMAKE_BINARY_DIR}/wav/ukulele.wav")
    endif()
endif()