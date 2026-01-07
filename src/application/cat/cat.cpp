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

#include <util/base/System.h>
#include <util/base/ArgumentParser.h>
#include <util/collection/Array.h>
#include <util/io/file/File.h>
#include <util/io/stream/BufferedInputStream.h>
#include <util/io/stream/FileInputStream.h>
#include <util/io/stream/PrintStream.h>
#include <util/io/stream/InputStream.h>

const char *HELP_TEXT =
#include "generated/README.md"
;

/// Read from the given input stream and write to standard output.
/// When a newline character is encountered, the output stream is flushed.
/// The input stream is read until EOF (-1) is reached.
void processStream(Util::Io::InputStream &stream) {
    int16_t c = stream.read();
    while (c >= 0) {
        Util::System::out << static_cast<char>(c);
        if (c == '\n') {
            Util::System::out << Util::Io::PrintStream::flush;
        }

        c = stream.read();
    }
}

int main(const int argc, char *argv[]) {
    Util::ArgumentParser argumentParser;
    argumentParser.setHelpText(HELP_TEXT);

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    const auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        processStream(Util::System::in);
    } else {
        for (const auto &path : arguments) {
            auto file = Util::Io::File(path);
            if (!file.exists()) {
                Util::System::error << "cat: '" << path << "' not found!" << Util::Io::PrintStream::lnFlush;
                continue;
            }

            if (file.isDirectory()) {
                Util::System::error << "cat: '" << path << "' is a directory!" << Util::Io::PrintStream::lnFlush;
                continue;
            }

            Util::Io::FileInputStream fileStream(file);
            Util::Io::BufferedInputStream bufferedStream(fileStream);
            auto &stream = file.getType() == Util::Io::File::REGULAR ?
                static_cast<Util::Io::InputStream&>(bufferedStream) :
                static_cast<Util::Io::InputStream&>(fileStream);

            processStream(stream);
        }
    }

    Util::System::out << Util::Io::PrintStream::flush;
    return 0;
}