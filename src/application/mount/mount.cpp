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
#include "lib/util/base/String.h"

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.addArgument("type", true, "t");
    argumentParser.setHelpText("Mount a device to a path.\n"
                               "Usage: mount [DEVICE] [PATH] [OPTIONS]...\n"
                               "Options:\n"
                               "  -t, --type: Filesystem type (REQUIRED).\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto type = argumentParser.getArgument("type");
    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() < 2) {
        Util::System::error << "mount: Too few arguments provided!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto success = Util::Io::File::mount(arguments[0], arguments[1], type);
    if (!success) {
        Util::System::error << "mount: Failed to mount '" << argv[1] << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    }

    return success ? 0 : -1;
}