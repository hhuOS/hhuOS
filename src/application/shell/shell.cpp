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

#include <string.h>
#include <stdint.h>

#include "lib/util/base/ArgumentParser.h"
#include "lib/util/base/System.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/io/stream/PrintStream.h"
#include "CommandLine.h"
#include "lib/util/base/String.h"

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("A simple UNIX-like shell.\n"
                               "Usage: shell [-h] [cwd] [-c command...]\n"
                               "Options:\n"
                               "  -h, --help: Show this help message\n");

    // Execute application and exit (required by libc `system()`)
    if (argc > 2) {
        for (int32_t i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-c") == 0) {
                Util::ArrayList<Util::String> commandParts = Util::ArrayList<Util::String>();
                for (int32_t j = i + 1; j < argc; j++) {
                    commandParts.add(Util::String(argv[j]));
                }

            Util::String command = Util::String::join(Util::String(" "), commandParts.toArray());
            CommandLine shell = CommandLine(argv[1]);
            shell.runCommand(command);
            return 0;
            }
        }
    }

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    CommandLine shell(argc > 1 ? argv[1] : "/");
    shell.run();

    return 0;
}
