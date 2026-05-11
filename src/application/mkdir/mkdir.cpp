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
#include <util/io/stream/PrintStream.h>

constexpr const char *HELP_TEXT =
#include "generated/README.md"
;

int32_t main(const int32_t argc, char *argv[]) {
    Util::ArgumentParser argumentParser;
    argumentParser.setHelpText(HELP_TEXT);
    argumentParser.addSwitch("verbose", "v");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    const auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "mkdir: No arguments provided!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    for (const auto &path : arguments) {
        const Util::Io::File file(path);
        if (file.exists()) {
            Util::System::error << "mkdir: '" << path << "' already exists!" << Util::Io::PrintStream::lnFlush;
            continue;
        }

        if (!file.create(Util::Io::File::DIRECTORY)) {
            Util::System::error << "mkdir: Failed to create directory '" << path << "'!" <<
                Util::Io::PrintStream::lnFlush;
        }

        if (argumentParser.checkSwitch("verbose")) {
            Util::System::out << "mkdir: Created directory '" << path << "'" << Util::Io::PrintStream::lnFlush;
        }
    }

    return 0;
}