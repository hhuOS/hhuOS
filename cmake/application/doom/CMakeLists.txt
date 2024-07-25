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

project(doom)
message(STATUS "Project " ${PROJECT_NAME})

include_directories(${HHUOS_SRC_DIR})

#add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/doom/"
 #       COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/doom/")
		
#add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/doom/DOOM.WAD"
#		COMMAND wget "https://archive.org/download/2020_03_22_DOOM/DOOM%20WADs/Doom%20%28v1.9%29.zip" -O "${CMAKE_BINARY_DIR}/doom/wad.zip"
#		COMMAND unzip -u -d "${CMAKE_BINARY_DIR}/doom" "${CMAKE_BINARY_DIR}/doom/wad.zip"
#		COMMAND rm "${CMAKE_BINARY_DIR}/doom/wad.zip"
#		DEPENDS "${CMAKE_BINARY_DIR}/doom/")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D FEATURE_SOUND")

# Set source files
set(SOURCE_FILES
        ${HHUOS_SRC_DIR}/application/doom/am_map.c
        ${HHUOS_SRC_DIR}/application/doom/d_event.c
        ${HHUOS_SRC_DIR}/application/doom/d_items.c
        ${HHUOS_SRC_DIR}/application/doom/d_iwad.c
        ${HHUOS_SRC_DIR}/application/doom/d_loop.c
        ${HHUOS_SRC_DIR}/application/doom/d_main.c
        ${HHUOS_SRC_DIR}/application/doom/d_mode.c
        ${HHUOS_SRC_DIR}/application/doom/d_net.c
        ${HHUOS_SRC_DIR}/application/doom/doomdef.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric_hhuos.cpp
        ${HHUOS_SRC_DIR}/application/doom/doomstat.c
        ${HHUOS_SRC_DIR}/application/doom/dstrings.c
        ${HHUOS_SRC_DIR}/application/doom/dummy.c
        ${HHUOS_SRC_DIR}/application/doom/f_finale.c
        ${HHUOS_SRC_DIR}/application/doom/f_wipe.c
        ${HHUOS_SRC_DIR}/application/doom/g_game.c
        ${HHUOS_SRC_DIR}/application/doom/gusconf.c
        ${HHUOS_SRC_DIR}/application/doom/hu_lib.c
        ${HHUOS_SRC_DIR}/application/doom/hu_stuff.c
        ${HHUOS_SRC_DIR}/application/doom/i_cdmus.c
        ${HHUOS_SRC_DIR}/application/doom/icon.c
        ${HHUOS_SRC_DIR}/application/doom/i_endoom.c
        ${HHUOS_SRC_DIR}/application/doom/i_input.c
        ${HHUOS_SRC_DIR}/application/doom/i_joystick.c
        ${HHUOS_SRC_DIR}/application/doom/info.c
        ${HHUOS_SRC_DIR}/application/doom/i_scale.c
        ${HHUOS_SRC_DIR}/application/doom/i_sound.c
        ${HHUOS_SRC_DIR}/application/doom/i_system.c
        ${HHUOS_SRC_DIR}/application/doom/i_timer.c
        ${HHUOS_SRC_DIR}/application/doom/i_video.c
        ${HHUOS_SRC_DIR}/application/doom/m_argv.c
        ${HHUOS_SRC_DIR}/application/doom/m_bbox.c
        ${HHUOS_SRC_DIR}/application/doom/m_cheat.c
        ${HHUOS_SRC_DIR}/application/doom/m_config.c
        ${HHUOS_SRC_DIR}/application/doom/m_controls.c
        ${HHUOS_SRC_DIR}/application/doom/memio.c
        ${HHUOS_SRC_DIR}/application/doom/m_fixed.c
        ${HHUOS_SRC_DIR}/application/doom/m_menu.c
        ${HHUOS_SRC_DIR}/application/doom/m_misc.c
        ${HHUOS_SRC_DIR}/application/doom/m_random.c
        ${HHUOS_SRC_DIR}/application/doom/mus2mid.c
        ${HHUOS_SRC_DIR}/application/doom/p_ceilng.c
        ${HHUOS_SRC_DIR}/application/doom/p_doors.c
        ${HHUOS_SRC_DIR}/application/doom/p_enemy.c
        ${HHUOS_SRC_DIR}/application/doom/p_floor.c
        ${HHUOS_SRC_DIR}/application/doom/p_inter.c
        ${HHUOS_SRC_DIR}/application/doom/p_lights.c
        ${HHUOS_SRC_DIR}/application/doom/p_map.c
        ${HHUOS_SRC_DIR}/application/doom/p_maputl.c
        ${HHUOS_SRC_DIR}/application/doom/p_mobj.c
        ${HHUOS_SRC_DIR}/application/doom/p_plats.c
        ${HHUOS_SRC_DIR}/application/doom/p_pspr.c
        ${HHUOS_SRC_DIR}/application/doom/p_saveg.c
        ${HHUOS_SRC_DIR}/application/doom/p_setup.c
        ${HHUOS_SRC_DIR}/application/doom/p_sight.c
        ${HHUOS_SRC_DIR}/application/doom/p_spec.c
        ${HHUOS_SRC_DIR}/application/doom/p_switch.c
        ${HHUOS_SRC_DIR}/application/doom/p_telept.c
        ${HHUOS_SRC_DIR}/application/doom/p_tick.c
        ${HHUOS_SRC_DIR}/application/doom/p_user.c
        ${HHUOS_SRC_DIR}/application/doom/r_bsp.c
        ${HHUOS_SRC_DIR}/application/doom/r_data.c
        ${HHUOS_SRC_DIR}/application/doom/r_draw.c
        ${HHUOS_SRC_DIR}/application/doom/r_main.c
        ${HHUOS_SRC_DIR}/application/doom/r_plane.c
        ${HHUOS_SRC_DIR}/application/doom/r_segs.c
        ${HHUOS_SRC_DIR}/application/doom/r_sky.c
        ${HHUOS_SRC_DIR}/application/doom/r_things.c
        ${HHUOS_SRC_DIR}/application/doom/sha1.c
        ${HHUOS_SRC_DIR}/application/doom/sounds.c
        ${HHUOS_SRC_DIR}/application/doom/s_sound.c
        ${HHUOS_SRC_DIR}/application/doom/statdump.c
        ${HHUOS_SRC_DIR}/application/doom/st_lib.c
        ${HHUOS_SRC_DIR}/application/doom/st_stuff.c
        ${HHUOS_SRC_DIR}/application/doom/tables.c
        ${HHUOS_SRC_DIR}/application/doom/v_video.c
        ${HHUOS_SRC_DIR}/application/doom/w_checksum.c
        ${HHUOS_SRC_DIR}/application/doom/w_file.c
        ${HHUOS_SRC_DIR}/application/doom/w_file_stdc.c
        ${HHUOS_SRC_DIR}/application/doom/wi_stuff.c
        ${HHUOS_SRC_DIR}/application/doom/w_main.c
        ${HHUOS_SRC_DIR}/application/doom/w_wad.c
        ${HHUOS_SRC_DIR}/application/doom/z_zone.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} lib.user.runtime lib.user.base lib.user.libc lib.sound)