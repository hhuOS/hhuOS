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
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/collection/Array.h"
#include "lib/util/io/file/File.h"
#include "lib/util/io/stream/BufferedInputStream.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/io/stream/InputStream.h"

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Concatenate multiple files on stdout.\n"
                               "Usage: cat [FILE]...\n"
                               "Options:\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "cat: No arguments provided!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    for (const auto &path : arguments) {
        auto file = Util::Io::File(path);
        if (!file.exists()) {
            Util::System::error << "cat: '" << path << "' not found!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            continue;
        }

        if (file.isDirectory()) {
            Util::System::error << "cat: '" << path << "' is a directory!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            continue;
        }

        auto fileStream = Util::Io::FileInputStream(file);
        auto bufferedStream = Util::Io::BufferedInputStream(fileStream);
        auto &stream = (file.getType() == Util::Io::File::REGULAR) ? static_cast<Util::Io::InputStream&>(bufferedStream) : static_cast<Util::Io::InputStream&>(fileStream);

        int16_t logChar = stream.read();
        while (logChar != -1) {
            Util::System::out << static_cast<char>(logChar) << Util::Io::PrintStream::flush;
            logChar = stream.read();
        }
    }

    Util::System::out << Util::Io::PrintStream::flush;
    return 0;
}