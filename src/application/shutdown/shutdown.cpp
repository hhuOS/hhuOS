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

#include "lib/util/system/Machine.h"
#include "lib/util/system/System.h"
#include "lib/util/ArgumentParser.h"
#include "lib/util/Exception.h"
#include "lib/util/stream/PrintWriter.h"

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.addSwitch("reboot", "r");
    argumentParser.setHelpText("Shut down the system.\n"
                               "Usage: shutdown [OPTIONS]...\n"
                               "Options:\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    Util::Machine::ShutdownType type = argumentParser.checkSwitch("reboot") ? Util::Machine::REBOOT : Util::Machine::SHUTDOWN;
    auto success = Util::Machine::shutdown(type);
    if (success) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Shutdown returned successfully!");
    }

    return -1;
}