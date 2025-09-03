/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include <stdint.h>

#include "lib/util/base/System.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/collection/Array.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/String.h"
#include "lib/util/io/stream/BufferedInputStream.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/io/stream/InputStream.h"

static const constexpr uint8_t LINE_LENGTH = 16;
static const constexpr char LINE_SEPARATOR = '-';
static const constexpr char *HEXDUMP_HEADER = "  OFFSET  | 00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F |   ANSI  ASCII   ";

void printSeparationLine() {
    auto length = Util::Address(HEXDUMP_HEADER).stringLength();
    for (uint32_t i = 0; i < length; i++) {
        Util::System::out << LINE_SEPARATOR;
    }

    Util::System::out << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
}

char sanitize(char value) {
    if (value < 0x20 || value > 0x7E) {
        return '.';
    }

    return value;
}

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.addArgument("length", false, "n");
    argumentParser.addArgument("skip", false, "s");
    argumentParser.setHelpText("Print file contents in hexadecimal numbers.\n"
                               "Usage: hexdump [FILE]\n"
                               "Options:\n"
                               "  -n, --length [LENGTH]: The maximum amount of bytes to read\n"
                               "  -s, --skip [POSITION]: Skip bytes from the beginning of the file to POSITION\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "hexdump: No arguments provided!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return -1;
    }

    auto path = Util::String(arguments[0]);
    auto file = Util::Io::File(path);
    if (!file.exists()) {
        Util::System::error << "hexdump: '" << path << "' not found!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return -1;
    }

    if (file.isDirectory()) {
        Util::System::error << "hexdump: '" << path << "' is a directory!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return -1;
    }

    auto fileStream = Util::Io::FileInputStream(file);
    auto bufferedStream = Util::Io::BufferedInputStream(fileStream);
    auto &stream = (file.getType() == Util::Io::File::REGULAR) ? static_cast<Util::Io::InputStream&>(bufferedStream) : static_cast<Util::Io::InputStream&>(fileStream);

    Util::System::out << Util::Io::PrintStream::hex << HEXDUMP_HEADER << Util::Io::PrintStream::ln;
    printSeparationLine();

    int32_t length = argumentParser.hasArgument("length") ? Util::String::parseNumber<int32_t>(argumentParser.getArgument("length")) : -1;
    int32_t readBytes = 0;
    uint32_t address = 0;

    if (argumentParser.hasArgument("skip")) {
        address = Util::String::parseNumber<uint32_t>(argumentParser.getArgument("skip"));
        stream.skip(address);
    }

    while (length == -1 || readBytes < length) {
        Util::System::out.setIntegerPrecision(8);
        Util::System::out << " " << address << " | ";
        Util::System::out.setIntegerPrecision(2);

        char line[LINE_LENGTH];
        uint8_t i;
        for (i = 0; i < LINE_LENGTH && (length == -1 || readBytes < length); i++) {
            auto c = stream.read();
            if (c == -1) {
                break;
            }

            line[i] = c;
            readBytes++;
            Util::System::out << static_cast<uint8_t>(line[i]) << (i == 7 ? "  " : " ");
        }

        for (uint8_t j = i; j < LINE_LENGTH; j++) {
            Util::System::out << (j == 7 ? "    " : "   ");
        }
        Util::System::out << "| ";

        for (uint8_t j = 0; j < i; j++) {
            Util::System::out << sanitize(line[j]);
        }

        address += LINE_LENGTH;
        Util::System::out << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;

        if (i < LINE_LENGTH) {
            break;
        }
    }

    return 0;
}