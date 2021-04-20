# Copyright (C) 2019 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
# Heinrich-Heine University
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

cmake_minimum_required(VERSION 3.7)
 
target_sources(application PUBLIC
        ${HHUOS_SRC_DIR}/application/shell/Shell.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Command.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Mathexpr.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Rmdir.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Pwd.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Shutdown.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Ps.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Asciimate.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Beep.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/BmpView.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Cat.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Cd.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Clear.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Date.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/DelPart.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/DelVdd.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/DiskInfo.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Echo.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Head.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/History.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Insmod.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Ip.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/License.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Ls.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Mkdir.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Mkfs.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/MkPart.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/MkPartTable.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/MkVdd.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Mount.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/NetworkTest.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Rm.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Tail.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Touch.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Umount.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/Uptime.cpp
        ${HHUOS_SRC_DIR}/application/shell/command/WavPlay.cpp)
