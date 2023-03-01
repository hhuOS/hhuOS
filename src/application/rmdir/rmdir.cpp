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
#include "lib/util/io/stream/PrintStream.h"

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Delete directories.\n"
                               "Usage: rm [DIRECTORY]...\n"
                               "Options:\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "rm: No arguments provided!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    for (const auto &path : arguments) {
        auto file = Util::Io::File(path);
        if (!file.exists()) {
            Util::System::out << "rmdir: '" << path << "' not found!" << Util::Io::PrintStream::endl;
            continue;
        }

        if (!file.isDirectory()) {
            Util::System::out << "rmdir: '" << path << "' is not a directory!" << Util::Io::PrintStream::endl;
            continue;
        }

        if (file.getChildren().length() > 0) {
            Util::System::out << "rmdir: '" << path << "' is not empty!" << Util::Io::PrintStream::endl;
            continue;
        }

        auto success = file.remove();
        if (!success) {
            Util::System::out << "rmdir: Failed to delete directory '" << path << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        }
    }

    Util::System::out << Util::Io::PrintStream::flush;
    return 0;
}