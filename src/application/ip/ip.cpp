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
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/base/ArgumentParser.h"
#include "Address.h"
#include "Route.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"
#include "lib/util/collection/ArrayList.h"

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Show and edit IPv4 addresses of network devices.\n"
                               "Usage: ip [COMMAND] [ARGUMENTS...]\n"
                               "Valid commands: { address [add | delete] [address]\n"
                               "                  route [add | delete] [target address] [device]\n"
                               "                  route [add | delete] [source address] [target address] [device]\n"
                               "                  route [add | delete] [source address] [target address] [next hop] [device] }\n"
                               "Options:\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "ip: No arguments given!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto commandArguments = Util::ArrayList<Util::String>(arguments);
    commandArguments.remove(arguments[0]);

    if (Util::String(Address::COMMAND).toLowerCase().beginsWith(arguments[0])) {
        auto address = Address(commandArguments.toArray());
        return address.parse();
    } else if (Util::String(Route::COMMAND).toLowerCase().beginsWith(arguments[0])) {
        auto route = Route(commandArguments.toArray());
        return route.parse();
    } else {
        Util::System::error << "ip: Invalid command '" << arguments[0] << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }
}