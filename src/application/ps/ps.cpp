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
#include <util/io/stream/FileInputStream.h>
#include <util/base/ArgumentParser.h>
#include <util/collection/Array.h>
#include <util/io/file/File.h>
#include <util/graphic/Ansi.h>
#include <util/base/String.h>
#include <util/io/stream/PrintStream.h>

constexpr const char *HELP_TEXT =
#include "generated/README.md"
;

int32_t main(const int32_t argc, char *argv[]) {
    Util::ArgumentParser argumentParser;
    argumentParser.setHelpText(HELP_TEXT);

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    const Util::Io::File processDirectory("/process");
    auto processRootPath = processDirectory.getCanonicalPath();

    // Print table header
    Util::System::out << Util::Graphic::Ansi::FOREGROUND_BRIGHT_YELLOW << "PID\tThreads\tName"
        << Util::Graphic::Ansi::FOREGROUND_DEFAULT << Util::Io::PrintStream::ln;

    // Iterate over all folder in '/process' (one folder for each active process)
    for (const auto &child : processDirectory.getChildren()) {
        auto processPath = child.getCanonicalPath();

        const Util::Io::File nameFile(processPath + "/name");
        Util::Io::FileInputStream nameStream(nameFile);

        const Util::Io::File threadCountFile(processPath + "/thread_count");
        Util::Io::FileInputStream threadCountStream(threadCountFile);

        Util::System::out << child.getName() << "\t"
            << threadCountStream.readString(threadCountFile.getLength() - 1) << "\t"
            << nameStream.readString(nameFile.getLength() - 1) << Util::Io::PrintStream::ln;
    }

    Util::System::out << Util::Io::PrintStream::flush;
    return 0;
}