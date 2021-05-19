# Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

cmake_minimum_required(VERSION 3.7)

target_sources(lib.util PUBLIC
        ${HHUOS_SRC_DIR}/lib/util/stream/BufferedInputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/stream/BufferedOutputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/stream/BufferedReader.cpp
        ${HHUOS_SRC_DIR}/lib/util/stream/BufferedWriter.cpp
        ${HHUOS_SRC_DIR}/lib/util/stream/ByteArrayOutputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/stream/ByteArrayInputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/stream/FileInputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/stream/FilterInputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/stream/FilterOutputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/stream/InputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/stream/InputStreamReader.cpp
        ${HHUOS_SRC_DIR}/lib/util/stream/OutputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/stream/OutputStreamWriter.cpp
        ${HHUOS_SRC_DIR}/lib/util/stream/PipedInputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/stream/PipedOutputStream.cpp
        ${HHUOS_SRC_DIR}/lib/util/stream/PrintWriter.cpp
        ${HHUOS_SRC_DIR}/lib/util/stream/Reader.cpp
        ${HHUOS_SRC_DIR}/lib/util/stream/TerminalOutputStream.cpp)
