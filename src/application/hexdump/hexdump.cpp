/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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
#include <stddef.h>

#include <util/base/System.h>
#include <util/base/Address.h>
#include <util/base/ArgumentParser.h>
#include <util/collection/Array.h>
#include <util/collection/Pair.h>
#include <util/io/file/File.h>
#include <util/base/String.h>
#include <util/io/stream/BufferedInputStream.h>
#include <util/io/stream/FileInputStream.h>
#include <util/io/stream/PrintStream.h>
#include <util/io/stream/InputStream.h>

const char *HELP_TEXT =
#include "generated/README.md"
;

constexpr uint8_t LINE_LENGTH = 16;
constexpr char LINE_SEPARATOR = '-';
const Util::String HEXDUMP_HEADER = "  OFFSET  | 00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F |   ANSI  ASCII   ";

void printSeparationLine() {
    for (size_t i = 0; i < HEXDUMP_HEADER.length(); i++) {
        Util::System::out << LINE_SEPARATOR;
    }

    Util::System::out << Util::Io::PrintStream::lnFlush;
}

char sanitize(const char value) {
    if (value < 0x20 || value > 0x7E) {
        return '.';
    }

    return value;
}

int main(const int argc, char *argv[]) {
    Util::ArgumentParser argumentParser;
    argumentParser.addArgument("length", false, "n");
    argumentParser.addArgument("skip", false, "s");
    argumentParser.setHelpText(HELP_TEXT);

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    const size_t skip = argumentParser.hasArgument("skip") ?
        Util::String::parseNumber<uint32_t>(argumentParser.getArgument("skip")) : 0;

    const int32_t maxBytes = argumentParser.hasArgument("length") ?
        Util::String::parseNumber<int32_t>(argumentParser.getArgument("length")) : -1;

    Util::ArrayList<Util::Io::InputStream*> streams;
    const auto arguments = argumentParser.getUnnamedArguments();

    if (arguments.length() == 0) {
        // No files specified -> Read from standard input
        streams.add(&Util::System::in);
    } else {
        // Check for the existence of each file and create input streams accordingly
        for (const auto &path : arguments) {
            auto file = Util::Io::File(path);
            if (!file.exists()) {
                Util::System::error << "hexdump: '" << path << "' not found!" << Util::Io::PrintStream::lnFlush;
                continue;
            }

            if (file.isDirectory()) {
                Util::System::error << "hexdump: '" << path << "' is a directory!" << Util::Io::PrintStream::lnFlush;
                continue;
            }

            Util::Io::InputStream *stream = new Util::Io::FileInputStream(file);
            if (file.getType() == Util::Io::File::REGULAR) {
                // Regular files are buffered for performance
                Util::Io::InputStream *bufferedStream = new Util::Io::BufferedInputStream(stream);
                streams.add(bufferedStream);
            } else {
                // Other file types are not buffered, as they may represent devices or pipes
                streams.add(stream);
            }
        }
    }

    Util::System::out << Util::Io::PrintStream::hex << HEXDUMP_HEADER << Util::Io::PrintStream::ln;
    printSeparationLine();

    int32_t readBytes = 0;
    size_t address = 0;
    size_t currentStreamIndex = 0;
    auto *stream = streams.get(currentStreamIndex);

    // Skip the requested number of bytes
    while (address < skip) {
        const auto toSkip = skip - address;
        const auto skipped = stream->skip(toSkip);
        address += skipped;

        if (skipped < toSkip) {
            if (++currentStreamIndex >= streams.size()) {
                break;
            }

            stream = streams.get(currentStreamIndex);
        }
    }

    // Read and print bytes until the last stream is fully read or the maximum number of bytes is reached
    while (maxBytes == -1 || readBytes < maxBytes) {
        Util::System::out.setIntegerPrecision(8);
        Util::System::out << " " << address << " | ";
        Util::System::out.setIntegerPrecision(2);

        char line[LINE_LENGTH];
        uint8_t i;

        // Read a line of bytes (up to LINE_LENGTH)
        for (i = 0; i < LINE_LENGTH && (maxBytes == -1 || readBytes < maxBytes); i++) {
            auto c = stream->read();
            while (c < 0) {
                // Current stream is exhausted -> Try to switch to the next one
                if (++currentStreamIndex >= streams.size()) {
                    // No more streams available
                    break;
                }

                stream = streams.get(currentStreamIndex);
                c = stream->read();
            }

            if (c < 0) {
                // No more data available from any stream
                break;
            }

            // Print byte in hexadecimal format and store it in line buffer for ASCII representation
            Util::System::out << static_cast<uint8_t>(c) << (i == 7 ? "  " : " ");
            line[i] = static_cast<char>(c);
            readBytes++;
        }

        // A line has been fully read -> Print ASCII representation
        for (uint8_t j = i; j < LINE_LENGTH; j++) {
            Util::System::out << (j == 7 ? "    " : "   ");
        }
        Util::System::out << "| ";

        for (uint8_t j = 0; j < i; j++) {
            Util::System::out << sanitize(line[j]);
        }

        address += LINE_LENGTH;
        Util::System::out << Util::Io::PrintStream::lnFlush;

        if (i < LINE_LENGTH) {
            // No more data available
            break;
        }
    }

    for (const auto *stream : streams) {
        if (stream != &Util::System::in) {
            delete stream;
        }
    }

    return 0;
}