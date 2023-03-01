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

void treeDirectory(const Util::String &path, uint32_t level) {
    auto file = Util::Io::File(path);
    if (!file.exists()) {
        Util::System::error << "tree: '" << path << "' not found!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return;
    }

    auto string = Util::String("|-");
    for (uint32_t i = 0; i < level; i++) {
        string += "-";
    }

    string += Util::Io::File::getTypeColor(file) + file.getName() + (file.isDirectory() ? "/" : "") + Util::Graphic::Ansi::FOREGROUND_DEFAULT + " ";
    Util::System::out << string << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;

    if (file.isDirectory()) {
        auto basePath = file.getCanonicalPath();
        for (const auto &child : file.getChildren()) {
            treeDirectory(basePath + "/" + child, level + 1);
        }
    }
}

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Print a directory tree.\n"
                               "Usage: tree [DIRECTORY]...\n"
                               "Options:\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        treeDirectory(Util::Io::File::getCurrentWorkingDirectory().getCanonicalPath(), 0);
    } else {
        for (uint32_t i = 0; i < arguments.length(); i++) {
            treeDirectory(arguments[i], 0);
            if (i < static_cast<uint32_t>(arguments.length() - 1)) {
                Util::System::out << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            }
        }
    }

    Util::System::out << Util::Io::PrintStream::flush;
    return 0;
}