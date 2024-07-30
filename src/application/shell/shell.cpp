/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <string.h>
#include <cstdint>

#include "lib/util/base/ArgumentParser.h"
#include "lib/util/base/System.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/io/stream/PrintStream.h"
#include "Shell.h"
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
            Shell shell = Shell(argv[1]);
            shell.runCommand(command);
            return 0;
            }
        }
    }

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    Shell shell(argc > 1 ? argv[1] : "/");
    shell.run();

    return 0;
}
