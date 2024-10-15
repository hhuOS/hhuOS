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

project(doomgeneric)
message(STATUS "Project " ${PROJECT_NAME})

include_directories(${HHUOS_SRC_DIR})

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/doom/"
       COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/doom/")
		
if ($ENV{HHUOS_REDISTRIBUTABLE_BUILD})
		add_custom_target(doom-wad DEPENDS "${CMAKE_BINARY_DIR}/doom/")
else()
	add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/doom/DOOM.WAD"
		COMMAND wget "https://archive.org/download/DoomsharewareEpisode/doom.ZIP/DOOM1.WAD" -O "${CMAKE_BINARY_DIR}/doom/DOOM.WAD"
		DEPENDS "${CMAKE_BINARY_DIR}/doom/")

	add_custom_target(doom-wad DEPENDS "${CMAKE_BINARY_DIR}/doom/DOOM.WAD")
endif()

add_compile_options(-w -D __HHUOS__)

# Uncomment to activate sound via PC Speaker (experimental)
# add_compile_options(-D FEATURE_SOUND)

# Set source files
set(SOURCE_FILES
		${HHUOS_SRC_DIR}/application/doom/doomgeneric_hhuos.cpp
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/am_map.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/d_event.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/d_items.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/d_iwad.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/d_loop.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/d_main.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/d_mode.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/d_net.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/doomdef.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/doomgeneric.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/doomstat.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/dstrings.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/dummy.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/f_finale.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/f_wipe.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/g_game.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/gusconf.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/hu_lib.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/hu_stuff.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/i_cdmus.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/icon.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/i_endoom.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/i_input.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/i_joystick.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/info.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/i_scale.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/i_sound.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/i_system.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/i_timer.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/i_video.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/m_argv.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/m_bbox.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/m_cheat.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/m_config.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/m_controls.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/memio.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/m_fixed.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/m_menu.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/m_misc.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/m_random.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/mus2mid.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/p_ceilng.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/p_doors.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/p_enemy.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/p_floor.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/p_inter.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/p_lights.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/p_map.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/p_maputl.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/p_mobj.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/p_plats.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/p_pspr.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/p_saveg.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/p_setup.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/p_sight.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/p_spec.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/p_switch.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/p_telept.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/p_tick.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/p_user.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/r_bsp.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/r_data.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/r_draw.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/r_main.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/r_plane.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/r_segs.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/r_sky.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/r_things.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/sha1.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/sounds.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/s_sound.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/statdump.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/st_lib.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/st_stuff.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/tables.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/v_video.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/w_checksum.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/w_file.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/w_file_stdc.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/wi_stuff.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/w_main.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/w_wad.c
        ${HHUOS_SRC_DIR}/application/doom/doomgeneric/doomgeneric/z_zone.c)

add_executable(doom ${SOURCE_FILES})

target_link_libraries(doom lib.user.runtime lib.user.base lib.user.sound lib.user.libc)