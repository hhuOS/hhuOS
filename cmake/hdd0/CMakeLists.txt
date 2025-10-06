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

project(hdd0)
message(STATUS "Project " ${PROJECT_NAME})

add_custom_command(OUTPUT "${HHUOS_ROOT_DIR}/hdd0.img"
        WORKING_DIRECTORY "${HHUOS_ROOT_DIR}/disk/hdd0/"
        COMMAND /bin/mkdir -p "bin/"
        COMMAND /bin/mkdir -p "user/"
		COMMAND /bin/mkdir -p "user/doom"
        COMMAND /bin/mkdir -p "media/floppy/"
        COMMAND /bin/mkdir -p "media/cdrom/"
        COMMAND /bin/cp "$<TARGET_FILE:shell>" "bin/shell"
        COMMAND /bin/cp "$<TARGET_FILE:asciimate>" "bin/asciimate"
        COMMAND /bin/cp "$<TARGET_FILE:battlespace>" "bin/battlespace"
        COMMAND /bin/cp "$<TARGET_FILE:beep>" "bin/beep"
        COMMAND /bin/cp "$<TARGET_FILE:bug>" "bin/bug"
        COMMAND /bin/cp "$<TARGET_FILE:cat>" "bin/cat"
		COMMAND /bin/cp "$<TARGET_FILE:classicube>" "bin/classicube"
		COMMAND /bin/cp "$<TARGET_FILE:clownmdemu>" "bin/clownmdemu"
        COMMAND /bin/cp "$<TARGET_FILE:cp>" "bin/cp"
		COMMAND /bin/cp "$<TARGET_FILE:ctest>" "bin/ctest"
        COMMAND /bin/cp "$<TARGET_FILE:date>" "bin/date"
        COMMAND /bin/cp "$<TARGET_FILE:demo>" "bin/demo"
        COMMAND /bin/cp "$<TARGET_FILE:dino>" "bin/dino"
		COMMAND /bin/cp "$<TARGET_FILE:doom>" "bin/doom"
        COMMAND /bin/cp "$<TARGET_FILE:echo>" "bin/echo"
        COMMAND /bin/cp "$<TARGET_FILE:head>" "bin/head"
        COMMAND /bin/cp "$<TARGET_FILE:hexdump>" "bin/hexdump"
        COMMAND /bin/cp "$<TARGET_FILE:ip>" "bin/ip"
        COMMAND /bin/cp "$<TARGET_FILE:keyboard>" "bin/keyboard"
        COMMAND /bin/cp "$<TARGET_FILE:kill>" "bin/kill"
		COMMAND /bin/cp "$<TARGET_FILE:litenes>" "bin/litenes"
        COMMAND /bin/cp "$<TARGET_FILE:ls>" "bin/ls"
        COMMAND /bin/cp "$<TARGET_FILE:membench>" "bin/membench"
        COMMAND /bin/cp "$<TARGET_FILE:mkdir>" "bin/mkdir"
        COMMAND /bin/cp "$<TARGET_FILE:mount>" "bin/mount"
		COMMAND /bin/cp "$<TARGET_FILE:nettest>" "bin/nettest"
		COMMAND /bin/cp "$<TARGET_FILE:peanut-gb>" "bin/peanut-gb"
        COMMAND /bin/cp "$<TARGET_FILE:ping>" "bin/ping"
		COMMAND /bin/cp "$<TARGET_FILE:portablegl>" "bin/portablegl"
        COMMAND /bin/cp "$<TARGET_FILE:play>" "bin/play"
        COMMAND /bin/cp "$<TARGET_FILE:ps>" "bin/ps"
        COMMAND /bin/cp "$<TARGET_FILE:pwd>" "bin/pwd"
		COMMAND /bin/cp "$<TARGET_FILE:quake>" "bin/quake"
        COMMAND /bin/cp "$<TARGET_FILE:rm>" "bin/rm"
        COMMAND /bin/cp "$<TARGET_FILE:rmdir>" "bin/rmdir"
		COMMAND /bin/cp "$<TARGET_FILE:rogue3d>" "bin/rogue3d"
        COMMAND /bin/cp "$<TARGET_FILE:shutdown>" "bin/shutdown"
        COMMAND /bin/cp "$<TARGET_FILE:smbios>" "bin/smbios"
		COMMAND /bin/cp "$<TARGET_FILE:tinygl>" "bin/tinygl"
        COMMAND /bin/cp "$<TARGET_FILE:touch>" "bin/touch"
        COMMAND /bin/cp "$<TARGET_FILE:tree>" "bin/tree"
        COMMAND /bin/cp "$<TARGET_FILE:uecho>" "bin/uecho"
        COMMAND /bin/cp "$<TARGET_FILE:unmount>" "bin/unmount"
        COMMAND /bin/cp "$<TARGET_FILE:uptime>" "bin/uptime"
        COMMAND /bin/cp "$<TARGET_FILE:view3d>" "bin/view3d"
        COMMAND /bin/cp -r "${CMAKE_BINARY_DIR}/asciimation" "user"
        COMMAND /bin/cp -r "${CMAKE_BINARY_DIR}/beep" "user"
        COMMAND /bin/cp -r "${CMAKE_BINARY_DIR}/books" "user"
		COMMAND /bin/cp -r "${CMAKE_BINARY_DIR}/classicube" "user"
		COMMAND /bin/cp -r "${CMAKE_BINARY_DIR}/doom" "user"
		COMMAND /bin/cp -r "${CMAKE_BINARY_DIR}/quake" "user"
		COMMAND /bin/cp -r "${CMAKE_BINARY_DIR}/roms" "user"
        COMMAND /bin/cp -r "${CMAKE_BINARY_DIR}/wav" "user"
		COMMAND /bin/rm -f "${CMAKE_BINARY_DIR}/part.img"
        COMMAND mkfs.fat -C "${CMAKE_BINARY_DIR}/part.img" $<IF:$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>,129024,96256>
        COMMAND mcopy -s -o -i "${CMAKE_BINARY_DIR}/part.img" * ::/
        COMMAND fallocate -l 1M "${CMAKE_BINARY_DIR}/fill.img"
        COMMAND /bin/cat "${CMAKE_BINARY_DIR}/fill.img" "${CMAKE_BINARY_DIR}/part.img" "${CMAKE_BINARY_DIR}/fill.img" > "${HHUOS_ROOT_DIR}/hdd0.img"
		COMMAND /bin/rm "${CMAKE_BINARY_DIR}/part.img" "${CMAKE_BINARY_DIR}/fill.img"
        COMMAND /bin/echo -e "'o\\nn\\np\\n1\\n2048\\n\\nt\\ne\\nw\\n'" | fdisk "${HHUOS_ROOT_DIR}/hdd0.img"
        DEPENDS asciimation-star-wars beep-files books-gutenberg classicube-resources doom-wad gameboy-roms megadrive-roms nes-roms quake-pak wav-files
				shell asciimate battlespace beep bug cat classicube cp ctest date demo dino doom echo head hexdump ip keyboard kill litenes ls membench mkdir mount nettest peanut-gb ping play portablegl ps pwd quake rm rmdir rogue3d shutdown smbios tinygl touch tree uecho unmount uptime view3d doom-wad)

add_custom_target(${PROJECT_NAME}
		DEPENDS asciimation-star-wars beep-files books-gutenberg classicube-resources doom-wad gameboy-roms megadrive-roms nes-roms quake-pak wav-files
				shell asciimate battlespace beep bug cat classicube clownmdemu cp ctest date demo dino doom echo head hexdump ip keyboard kill litenes ls membench mkdir mount nettest peanut-gb ping play portablegl ps pwd quake rm rmdir rogue3d shutdown smbios tinygl touch tree uecho unmount uptime view3d
		"${HHUOS_ROOT_DIR}/hdd0.img")
