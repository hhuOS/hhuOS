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

#include "lib/util/graphic/Ansi.h"
#include "lib/util/base/System.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/collection/Array.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/String.h"
#include "lib/util/io/stream/PrintStream.h"

void lsDirectory(const Util::String &path) {
    auto file = Util::Io::File(path);
    if (!file.exists()) {
        Util::System::error << "ls: '" << path << "' not found!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return;
    }

    auto string = Util::String();
    if (file.isDirectory()) {
        for (const auto &child : file.getChildren()) {
            auto currentFile = Util::Io::File(file.getCanonicalPath() + "/" + child);
            string += Util::Io::File::getTypeColor(currentFile) + currentFile.getName() + (currentFile.isDirectory() ? "/" : "") + Util::Graphic::Ansi::FOREGROUND_DEFAULT + " ";
        }

        string = string.substring(0, string.length() - 1);
    } else {
        string += Util::Graphic::Ansi::FOREGROUND_BRIGHT_YELLOW + file.getName() + Util::Graphic::Ansi::FOREGROUND_DEFAULT;
    }

    if (!string.isEmpty()) {
        Util::System::out << string << Util::Io::PrintStream::endl;
    }
    Util::System::out << Util::Io::PrintStream::flush;
}

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("List files.\n"
                               "Usage: ls [PATH]...\n"
                               "Options:\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        lsDirectory(Util::Io::File::getCurrentWorkingDirectory().getCanonicalPath());
    } else {
        if (arguments.length() == 1) {
            lsDirectory(arguments[0]);
        } else {
            for (uint32_t i = 0; i < arguments.length(); i++) {
                Util::System::out << arguments[i] << ":" << Util::Io::PrintStream::endl;
                lsDirectory(arguments[i]);
                if (i < static_cast<uint32_t>(arguments.length() - 1)) {
                    Util::System::out << Util::Io::PrintStream::endl;
                }
            }
        }
    }

    Util::System::out << Util::Io::PrintStream::flush;
    return 0;
}