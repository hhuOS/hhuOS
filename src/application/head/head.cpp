/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/base/System.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/collection/Array.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/String.h"
#include "lib/util/io/stream/BufferedInputStream.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/io/stream/InputStream.h"

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.addArgument("bytes", false, "c");
    argumentParser.addArgument("lines", false, "n");
    argumentParser.setHelpText("Print the first 10 lines of each file.\n"
                               "Usage: head [OPTION]... [FILE]...\n"
                               "Options:\n"
                               "  -c, --bytes [COUNT]: Print the first COUNT bytes.\n"
                               "  -n, --lines [COUNT]: Print the first COUNT lines.\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "head: No arguments provided!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    bool byteMode = false;
    uint32_t count = 10;
    if (argumentParser.hasArgument("bytes")) {
        byteMode = true;
        count = Util::String::parseInt(argumentParser.getArgument("bytes"));
    } else if (argumentParser.hasArgument("lines")) {
        count = Util::String::parseInt(argumentParser.getArgument("lines"));
    }

    for (const auto &path : arguments) {
        auto file = Util::Io::File(path);
        if (!file.exists()) {
            Util::System::error << "head: '" << path << "' not found!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            continue;
        }

        if (file.isDirectory()) {
            Util::System::error << "head: '" << path << "' is a directory!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            continue;
        }

        if (arguments.length() > 1) {
            Util::System::out << "==> " << file.getName() << " <==" << Util::Io::PrintStream::endl << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        }

        auto fileStream = Util::Io::FileInputStream(file);
        auto bufferedStream = Util::Io::BufferedInputStream(fileStream);
        auto &stream = (file.getType() == Util::Io::File::REGULAR) ? static_cast<Util::Io::InputStream&>(bufferedStream) : static_cast<Util::Io::InputStream&>(fileStream);

        if (byteMode) {
            auto c = stream.read();
            for (uint32_t i = 0; i < count && c != -1; i++) {
                Util::System::out << static_cast<char>(c) << Util::Io::PrintStream::flush;
                c = stream.read();
            }
        } else {
            uint32_t lineCount = 0;
            auto c = stream.read();
            while (lineCount < count && c != -1) {
                Util::System::out << static_cast<char>(c) << Util::Io::PrintStream::flush;
                if (c == '\n') {
                    lineCount++;
                }
                c = stream.read();
            }
        }

        Util::System::out << Util::Io::PrintStream::flush;
    }

    return 0;
}