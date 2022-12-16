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
#include "lib/util/time/Timestamp.h"
#include "lib/util/ArgumentParser.h"
#include "lib/util/memory/String.h"
#include "lib/util/stream/PrintWriter.h"

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Print the system uptime.\n"
                               "Usage: uptime\n"
                               "Options:\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    auto systemTime = Util::Time::getSystemTime();
    if (systemTime.toSeconds() < 60) {
        Util::System::out << Util::Memory::String::format("%d", systemTime.toSeconds());
    } else if (systemTime.toSeconds() < 3600) {
        Util::System::out << Util::Memory::String::format("%d:%02d", systemTime.toMinutes(), systemTime.toSeconds() % 60);
    } else if (systemTime.toSeconds() < 86400 ) {
        auto seconds = systemTime.toSeconds() - (systemTime.toMinutes() * 60);
        Util::System::out << Util::Memory::String::format("%d:%02d:%02d", systemTime.toHours(), systemTime.toMinutes() % 60, seconds);
    } else {
        auto minutes = systemTime.toMinutes() - (systemTime.toHours() * 60);
        auto seconds = systemTime.toSeconds() - (systemTime.toMinutes() * 60);
        Util::System::out << Util::Memory::String::format("%d %s, %d:%02d:%02d", systemTime.toDays() == 1 ? "day" : "days", systemTime.toDays(), systemTime.toHours() % 24, minutes, systemTime.toSeconds() % seconds);
    }

    Util::System::out << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
    return 0;
}