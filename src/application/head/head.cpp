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

#include <stddef.h>

#include <lib/util/base/System.h>
#include <lib/util/base/ArgumentParser.h>
#include <lib/util/collection/Array.h>
#include <lib/util/io/file/File.h>
#include <lib/util/base/String.h>
#include <lib/util/io/stream/BufferedInputStream.h>
#include <lib/util/io/stream/FileInputStream.h>
#include <lib/util/io/stream/PrintStream.h>
#include <lib/util/io/stream/InputStream.h>

const char *HELP_TEXT =
#include "generated/README.md"
;

/// Read the first 'count' bytes from the given input stream and write to standard output.
/// The input stream is read until the specified count of bytes is reached or EOF (-1) is encountered.
void processBytes(Util::Io::InputStream &stream, size_t count) {
    if (count == 0) {
        return;
    }

    int16_t c = stream.read();
    while (c >= 0 && count > 0) {
        Util::System::out << static_cast<char>(c);
        if (c == '\n') {
            Util::System::out << Util::Io::PrintStream::flush;
        }

        count--;
        c = stream.read();
    }
}

/// Read the first 'count' lines from the given input stream and write to standard output.
/// A line is considered to be terminated by a newline character ('\n').
/// The input stream is read until the specified count of lines is reached or EOF (-1) is encountered.
void processLines(Util::Io::InputStream &stream, size_t count) {
    if (count == 0) {
        return;
    }

    int16_t c = stream.read();
    while (c >= 0 && count > 0) {
        Util::System::out << static_cast<char>(c);
        if (c == '\n') {
            Util::System::out << Util::Io::PrintStream::flush;
            count--;
        }

        c = stream.read();
    }
}

/// Read from the given input stream and write to standard output.
/// Depending on the byteMode flag, either the first 'count' bytes or the first 'count' lines are read.
/// The input stream is read until the specified count is reached or EOF (-1) is encountered.
void processStream(Util::Io::InputStream &stream, const bool byteMode, const size_t count) {
    if (byteMode) {
        processBytes(stream, count);
    } else {
        processLines(stream, count);
    }
}

int main(const int argc, char *argv[]) {
    Util::ArgumentParser argumentParser;
    argumentParser.addArgument("bytes", false, "c");
    argumentParser.addArgument("lines", false, "n");
    argumentParser.setHelpText(HELP_TEXT);

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    bool byteMode = false;
    uint32_t count = 10;
    if (argumentParser.hasArgument("bytes")) {
        byteMode = true;
        count = Util::String::parseNumber<uint32_t>(argumentParser.getArgument("bytes"));
    } else if (argumentParser.hasArgument("lines")) {
        count = Util::String::parseNumber<uint32_t>(argumentParser.getArgument("lines"));
    }

    const auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        processStream(Util::System::in, byteMode, count);
    } else {
        for (const auto &path : arguments) {
            auto file = Util::Io::File(path);
            if (!file.exists()) {
                Util::System::error << "head: '" << path << "' not found!" << Util::Io::PrintStream::lnFlush;
                continue;
            }

            if (file.isDirectory()) {
                Util::System::error << "head: '" << path << "' is a directory!" << Util::Io::PrintStream::lnFlush;
                continue;
            }

            if (arguments.length() > 1) {
                Util::System::out << "==> " << file.getName() << " <==" << Util::Io::PrintStream::lnFlush;
            }

            Util::Io::FileInputStream fileStream(file);
            Util::Io::BufferedInputStream bufferedStream(fileStream);
            auto &stream = file.getType() == Util::Io::File::REGULAR ?
                static_cast<Util::Io::InputStream&>(bufferedStream) :
                static_cast<Util::Io::InputStream&>(fileStream);

            processStream(stream, byteMode, count);

            if (arguments.length() > 1) {
                Util::System::out << Util::Io::PrintStream::lnFlush;
            }
        }
    }

    return 0;
}