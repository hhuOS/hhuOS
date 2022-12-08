/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <cstdint>
#include "lib/util/system/System.h"
#include "lib/util/memory/Address.h"

static const constexpr uint8_t LINE_LENGTH = 16;
static const constexpr char LINE_SEPARATOR = '-';
static const constexpr char *HEXDUMP_HEADER = "  OFFSET  | 00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F |   ANSI ASCII   ";

void printSeparationLine() {
    auto length = Util::Memory::Address<uint32_t>(HEXDUMP_HEADER).stringLength();
    for (uint32_t i = 0; i < length; i++) {
        Util::System::out << LINE_SEPARATOR;
    }

    Util::System::out << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
}

char sanitize(char value) {
    if (value < 0x30 || value > 0x7E) {
        return '.';
    }

    return value;
}

int32_t main(int32_t argc, char *argv[]) {
    if (argc < 2) {
        Util::System::error << "hexdump: No arguments provided!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    auto path = Util::Memory::String(argv[1]);
    auto file = Util::File::File(path);
    if (!file.exists()) {
        Util::System::error << "hexdump: '" << path << "' not found!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    if (file.isDirectory()) {
        Util::System::error << "hexdump: '" << path << "' is a directory!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    auto stream = Util::Stream::FileInputStream(file);
    auto bufferedStream = Util::Stream::BufferedInputStream(stream);

    Util::System::out << Util::Stream::PrintWriter::hex << HEXDUMP_HEADER << Util::Stream::PrintWriter::endl;
    printSeparationLine();

    uint32_t address = 0;

    while (true) {
        Util::System::out.setNumberPadding(8);
        Util::System::out << " " << address << " | ";
        Util::System::out.setNumberPadding(2);

        char line[LINE_LENGTH];
        uint8_t i;
        for (i = 0; i < LINE_LENGTH; i++) {
            int16_t c = bufferedStream.read();
            if (c == -1) {
                break;
            }

            line[i] = c;
            Util::System::out << static_cast<uint8_t>(line[i]) << (i == 7 ? "  " : " ");
        }

        for (uint8_t j = i; j < LINE_LENGTH; j++) {
            Util::System::out << (j == LINE_LENGTH - 1 ? "    " : "   ");
        }
        Util::System::out << "| ";

        for (uint8_t j = 0; j < i; j++) {
            Util::System::out << sanitize(line[j]);
        }

        address += LINE_LENGTH;
        Util::System::out << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;

        if (i < LINE_LENGTH) {
            return 0;
        }
    }
}