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

#include "lib/util/base/System.h"
#include "lib/util/io/stream/FileReader.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/collection/Array.h"
#include "lib/util/io/file/File.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/base/String.h"
#include "lib/util/io/stream/PrintWriter.h"

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Print running processes.\n"
                               "Usage: ps\n"
                               "Options:\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintWriter::endl << Util::Io::PrintWriter::flush;
        return -1;
    }

    auto processDirectory = Util::Io::File("/process");
    auto processRootPath = processDirectory.getCanonicalPath();

    Util::System::out << Util::Graphic::Ansi::FOREGROUND_BRIGHT_YELLOW << "PID\tThreads\tName" << Util::Graphic::Ansi::FOREGROUND_DEFAULT << Util::Io::PrintWriter::endl;
    for (const auto &child : processDirectory.getChildren()) {
        auto processPath = processRootPath + "/" + child + "/";

        auto nameFile = Util::Io::File(processPath + "/name");
        auto nameStream = Util::Io::FileReader(nameFile);

        auto threadCountFile = Util::Io::File(processPath + "/thread_count");
        auto threadCountStream = Util::Io::FileReader(threadCountFile);

        Util::System::out << child << "\t"
                        << threadCountStream.read(threadCountFile.getLength() -1)  << "\t"
                        << nameStream.read(nameFile.getLength() - 1) << Util::Io::PrintWriter::endl;
    }

    Util::System::out << Util::Io::PrintWriter::flush;
    return 0;
}