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
#include <util/time/Timestamp.h>
#include <util/base/ArgumentParser.h>
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

    const auto time = Util::Time::Timestamp::getSystemTime();
    if (time.toSeconds() < 60) {
        Util::System::out << Util::String::format("%d", time.toSeconds());
    } else if (time.toSeconds() < 3600) {
        Util::System::out << Util::String::format("%d:%02d", time.toMinutes(), time.toSeconds() % 60);
    } else if (time.toSeconds() < 86400 ) {
        const auto seconds = time.toSeconds() - time.toMinutes() * 60;
        Util::System::out << Util::String::format("%d:%02d:%02d", time.toHours(), time.toMinutes() % 60, seconds);
    } else {
        const auto minutes = time.toMinutes() - time.toHours() * 60;
        const auto seconds = time.toSeconds() - time.toMinutes() * 60;
        Util::System::out << Util::String::format("%d %s, %d:%02d:%02d", time.toDays() == 1 ? "day" : "days",
            time.toDays(), time.toHours() % 24, minutes, time.toSeconds() % seconds);
    }

    Util::System::out << Util::Io::PrintStream::lnFlush;
    return 0;
}