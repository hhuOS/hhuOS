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

project(quakegeneric)
message(STATUS "Project " ${PROJECT_NAME})

include_directories(${HHUOS_SRC_DIR} ${HHUOS_SRC_DIR}/lib/libc ${HHUOS_SRC_DIR}/lib/util)

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/quake/id1"
        COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/quake/id1")

if ($ENV{HHUOS_REDISTRIBUTABLE_BUILD})
    add_custom_target(quake-pak DEPENDS
            "${CMAKE_BINARY_DIR}/quake/id1")
else()
    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/quake/id1/pak0.pak"
            # COMMAND wget "https://archive.org/download/quake_pak_202306/quake_pak.zip/pak0.pak" -O "${CMAKE_BINARY_DIR}/quake/id1/pak0.pak"
            COMMAND wget "https://github.com/pweil-/origin-quake/raw/refs/heads/master/id1/pak0.pak" -O "${CMAKE_BINARY_DIR}/quake/id1/pak0.pak"
            COMMAND /bin/touch -a -m "${CMAKE_BINARY_DIR}/quake/id1/pak0.pak"
            COMMAND /bin/echo "5906e5998fc3d896ddaf5e6a62e03abb ${CMAKE_BINARY_DIR}/quake/id1/pak0.pak" > "pak0.pak.md5"
            COMMAND md5sum -c "pak0.pak.md5"
            DEPENDS "${CMAKE_BINARY_DIR}/quake/id1")

    add_custom_target(quake-pak DEPENDS
            "${CMAKE_BINARY_DIR}/quake/id1/pak0.pak")
endif()

add_compile_options(
        $<$<COMPILE_LANGUAGE:C>:-std=c17>
        $<$<COMPILE_LANGUAGE:C>:-O0> # Quake does not work with compiler optimizations
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-parameter>
        $<$<COMPILE_LANGUAGE:C>:-Wno-missing-field-initializers>
        $<$<COMPILE_LANGUAGE:C>:-Wno-dangling-else>
        $<$<COMPILE_LANGUAGE:C>:-Wno-sign-compare>
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-but-set-variable>
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-parameter>
        $<$<COMPILE_LANGUAGE:C>:-Wno-maybe-uninitialized>
        $<$<COMPILE_LANGUAGE:C>:-Wno-implicit-fallthrough>
        $<$<COMPILE_LANGUAGE:C>:-Wno-misleading-indentation>
        $<$<COMPILE_LANGUAGE:C>:-Wno-pointer-sign>
        $<$<COMPILE_LANGUAGE:C>:-Wno-array-parameter>
        $<$<COMPILE_LANGUAGE:C>:-Wno-dangling-pointer>
        $<$<COMPILE_LANGUAGE:C>:-Wno-shift-overflow>
        $<$<COMPILE_LANGUAGE:C>:-Wno-strict-aliasing>
        $<$<COMPILE_LANGUAGE:C>:-Wno-aggressive-loop-optimizations>
)

set(SOURCE_FILES
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric_hhuos.cpp
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/cd_null.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/chase.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/cl_demo.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/cl_input.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/cl_main.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/cl_parse.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/cl_tent.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/cmd.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/common.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/console.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/crc.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/cvar.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/d_edge.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/d_fill.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/d_init.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/d_modech.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/d_part.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/d_polyse.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/draw.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/d_scan.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/d_sky.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/d_sprite.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/d_surf.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/d_vars.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/d_zpoint.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/host.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/host_cmd.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/in_null.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/keys.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/mathlib.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/menu.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/model.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/net_loop.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/net_main.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/net_none.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/net_vcr.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/nonintel.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/pr_cmds.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/pr_edict.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/pr_exec.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/quakegeneric.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/r_aclip.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/r_alias.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/r_bsp.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/r_draw.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/r_edge.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/r_efrag.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/r_light.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/r_main.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/r_misc.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/r_part.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/r_sky.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/r_sprite.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/r_surf.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/r_vars.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/sbar.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/screen.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/snd_null.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/sv_main.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/sv_move.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/sv_phys.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/sv_user.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/sys_null.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/vid_null.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/view.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/wad.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/world.c
        ${HHUOS_SRC_DIR}/application/quake/quakegeneric/source/zone.c)

add_executable(quake ${SOURCE_FILES})

target_link_libraries(quake lib.user.runtime lib.user.base lib.user.graphic lib.user.libc)