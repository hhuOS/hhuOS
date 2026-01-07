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

#include <lib/util/base/System.h>
#include <lib/util/time/Date.h>
#include <lib/util/base/ArgumentParser.h>
#include <lib/util/base/String.h>
#include <lib/util/io/stream/PrintStream.h>

const char *HELP_TEXT =
#include "generated/README.md"
;

int main(const int argc, char *argv[]) {
    Util::ArgumentParser argumentParser;
    argumentParser.setHelpText(HELP_TEXT);

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    const auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() > 1) {
        Util::System::error << "date: Too many arguments!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    const char *format = arguments.length() == 0 ? "%c" : static_cast<const char*>(arguments[0]);
    const Util::Time::Date now;

    Util::System::out << Util::String::formatDate(now, format) << Util::Io::PrintStream::lnFlush;
    return 0;
}