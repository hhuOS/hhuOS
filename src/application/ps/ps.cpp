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
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/collection/Array.h"
#include "lib/util/io/file/File.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/base/String.h"
#include "lib/util/io/stream/PrintStream.h"

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Print running processes.\n"
                               "Usage: ps\n"
                               "Options:\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto processDirectory = Util::Io::File("/process");
    auto processRootPath = processDirectory.getCanonicalPath();

    Util::System::out << Util::Graphic::Ansi::FOREGROUND_BRIGHT_YELLOW << "PID\tThreads\tName" << Util::Graphic::Ansi::FOREGROUND_DEFAULT << Util::Io::PrintStream::endl;
    for (const auto &child : processDirectory.getChildren()) {
        auto processPath = processRootPath + "/" + child + "/";

        auto nameFile = Util::Io::File(processPath + "/name");
        auto nameStream = Util::Io::FileInputStream(nameFile);

        auto threadCountFile = Util::Io::File(processPath + "/thread_count");
        auto threadCountStream = Util::Io::FileInputStream(threadCountFile);

        Util::System::out << child << "\t"
                          << threadCountStream.readString(threadCountFile.getLength() - 1) << "\t"
                          << nameStream.readString(nameFile.getLength() - 1) << Util::Io::PrintStream::endl;
    }

    Util::System::out << Util::Io::PrintStream::flush;
    return 0;
}