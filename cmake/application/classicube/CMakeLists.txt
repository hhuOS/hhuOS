# Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

project(classicube)
message(STATUS "Project " ${PROJECT_NAME})

set(CLASSICUBE_OPTIONS "\
window-width=320\\n\
window-height=200\\n\
viewdist=8\\n\
gfx-smoothlighting=false\\n\
gfx-maxchunkupdates=4\\n\
camera-smooth=false\\n\
normal=normalfast\\n\
entityshadow=None\\n")

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/classicube"
        COMMAND /bin/mkdir -p "${CMAKE_BINARY_DIR}/classicube")

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/classicube/options.txt"
        COMMAND /bin/echo -e "'${CLASSICUBE_OPTIONS}'" | sed -r "s/[\\\\]+//g" > "${CMAKE_BINARY_DIR}/classicube/options.txt")

add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/classicube/texpacks/classicube.zip"
        COMMAND wget "https://cdn.classicube.net/client/release/nix64/ClassiCube.tar.gz" -O "${CMAKE_BINARY_DIR}/classicube.tar.gz"
        COMMAND tar -xzf "${CMAKE_BINARY_DIR}/classicube.tar.gz" -C "${CMAKE_BINARY_DIR}"
        COMMAND /bin/cp -r "${CMAKE_BINARY_DIR}/ClassiCube/texpacks/" "${CMAKE_BINARY_DIR}/classicube/"
        COMMAND /bin/rm -r "${CMAKE_BINARY_DIR}/ClassiCube" "${CMAKE_BINARY_DIR}/classicube.tar.gz")

add_custom_target(classicube-resources DEPENDS
        "${CMAKE_BINARY_DIR}/classicube/options.txt"
        "${CMAKE_BINARY_DIR}/classicube/texpacks/classicube.zip")

add_compile_options(
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-parameter>
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-function>
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-but-set-variable>
        $<$<COMPILE_LANGUAGE:C>:-Wno-maybe-uninitialized>
        $<$<COMPILE_LANGUAGE:C>:-Wno-missing-field-initializers>
        $<$<COMPILE_LANGUAGE:C>:-Wno-sign-compare>
        $<$<COMPILE_LANGUAGE:C>:-Wno-cast-function-type>
        $<$<COMPILE_LANGUAGE:C>:-Wno-implicit-fallthrough>
        $<$<COMPILE_LANGUAGE:C>:-Wno-old-style-declaration>
        $<$<COMPILE_LANGUAGE:C>:-Wno-type-limits>
        $<$<COMPILE_LANGUAGE:C>:-Wno-missing-braces>
        $<$<COMPILE_LANGUAGE:C>:-Wno-dangling-pointer>
        $<$<COMPILE_LANGUAGE:C>:-Wno-strict-aliasing>
        $<$<COMPILE_LANGUAGE:C>:-DCC_BUILD_MANUAL>
        $<$<COMPILE_LANGUAGE:C>:-DCC_BUILD_COOPTHREADED>
        $<$<COMPILE_LANGUAGE:C>:-DCC_BUILD_NOMUSIC>
        $<$<COMPILE_LANGUAGE:C>:-DCC_BUILD_NOSOUNDS>
        $<$<COMPILE_LANGUAGE:C>:-DCC_GFX_BACKEND=CC_GFX_BACKEND_SOFTGPU>
        $<$<COMPILE_LANGUAGE:C>:-DCC_NET_BACKEND=CC_NET_BACKEND_BUILTIN>
        $<$<COMPILE_LANGUAGE:C>:-DCC_SSL_BACKEND=CC_SSL_BACKEND_NONE>
)

set(SOURCE_FILES
        ${HHUOS_SRC_DIR}/application/classicube/Window_hhuOS.cpp
        ${HHUOS_SRC_DIR}/application/classicube/Platform_hhuOS.cpp
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Animations.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/AudioBackend.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Audio.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/_autofit.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/AxisLinesRenderer.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Bitmap.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Block.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/BlockPhysics.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Builder.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Camera.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/_cff.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Chat.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Commands.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Deflate.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Drawer2D.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Drawer.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Entity.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/EntityComponents.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/EntityRenderers.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/EnvRenderer.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Event.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/ExtMath.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/FancyLighting.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Formats.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/_ftbase.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/_ftbitmap.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/_ftglyph.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/_ftinit.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/_ftsynth.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Game.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/GameVersion.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Generator.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Graphics_SoftGPU.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Gui.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/HeldBlockRenderer.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Http_Web.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Http_Worker.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Input.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/InputHandler.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Inventory.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/IsometricDrawer.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Launcher.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/LBackend_Android.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/LBackend.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Lighting.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Logger.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/LScreens.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/LWeb.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/LWidgets.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/main.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/MapRenderer.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/MenuOptions.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Menus.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Model.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Options.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/PackedCol.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Particle.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Physics.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Picking.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Protocol.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/_psaux.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/_pshinter.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/_psmodule.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Queue.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Resources.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Screens.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/SelectionBox.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/SelOutlineRenderer.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Server.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/_sfnt.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/_smooth.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/SSL.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Stream.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/String.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/SystemFonts.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/TexturePack.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/TouchUI.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/_truetype.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/_type1.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Utils.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Vectors.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Vorbis.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/Widgets.c
        ${HHUOS_SRC_DIR}/application/classicube/ClassiCube/src/World.c)

include_directories(${HHUOS_SRC_DIR} ${HHUOS_SRC_DIR}/lib/libc ${HHUOS_SRC_DIR}/lib/util)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} lib.user.runtime lib.user.base)