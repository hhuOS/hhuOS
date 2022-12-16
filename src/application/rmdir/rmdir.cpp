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
#include "lib/util/ArgumentParser.h"
#include "lib/util/data/Array.h"
#include "lib/util/file/File.h"
#include "lib/util/stream/PrintWriter.h"

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Delete directories.\n"
                               "Usage: rm [DIRECTORY]...\n"
                               "Options:\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "rm: No arguments provided!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    for (const auto &path : arguments) {
        auto file = Util::File::File(path);
        if (!file.exists()) {
            Util::System::out << "rmdir: '" << path << "' not found!" << Util::Stream::PrintWriter::endl;
            continue;
        }

        if (!file.isDirectory()) {
            Util::System::out << "rmdir: '" << path << "' is not a directory!" << Util::Stream::PrintWriter::endl;
            continue;
        }

        if (file.getChildren().length() > 0) {
            Util::System::out << "rmdir: '" << path << "' is not empty!" << Util::Stream::PrintWriter::endl;
            continue;
        }

        auto success = file.remove();
        if (!success) {
            Util::System::out << "rmdir: Failed to delete directory '" << path << "'!" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        }
    }

    Util::System::out << Util::Stream::PrintWriter::flush;
    return 0;
}