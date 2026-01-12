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
#include <stdlib.h>
#include <string.h>

#include <lib/util/graphic/Ansi.h>
#include <lib/util/base/System.h>
#include <lib/util/base/ArgumentParser.h>
#include <lib/util/collection/Array.h>
#include <lib/util/io/file/File.h>
#include <lib/util/base/String.h>
#include <lib/util/io/stream/PrintStream.h>

int compareFileNames(const void *a, const void *b) {
    const auto &fileA = *static_cast<const Util::Io::File*>(a);
    const auto &fileB = *static_cast<const Util::Io::File*>(b);
    return strcmp(static_cast<const char*>(fileA.getName()), static_cast<const char*>(fileB.getName()));
}

const char* getTypeColor(const Util::Io::File &file) {
    switch (file.getType()) {
        case Util::Io::File::DIRECTORY:
            return Util::Graphic::Ansi::FOREGROUND_BRIGHT_BLUE;
        case Util::Io::File::REGULAR:
            return Util::Graphic::Ansi::FOREGROUND_WHITE;
        case Util::Io::File::CHARACTER:
            return Util::Graphic::Ansi::FOREGROUND_BRIGHT_YELLOW;
        case Util::Io::File::SYSTEM:
            return Util::Graphic::Ansi::FOREGROUND_BRIGHT_GREEN;
        default:
            return Util::Graphic::Ansi::FOREGROUND_WHITE;
    }
}

Util::String formatFileName(const Util::Io::File &file) {
    return getTypeColor(file) + file.getName() +
        (file.isDirectory() ? "/" : "") + Util::Graphic::Ansi::FOREGROUND_DEFAULT;
}

void lsDirectory(const Util::String &path) {
    const auto file = Util::Io::File(path);
    if (!file.exists()) {
        Util::System::error << "ls: '" << path << "' not found!" << Util::Io::PrintStream::lnFlush;
        return;
    }

    if (file.isDirectory()) {
        const auto children = file.getChildren();
        Util::Array<Util::String> fileNames(children.length());

        for (size_t i = 0; i < children.length(); i++) {
            const auto &child = children[i];
            fileNames[i] += formatFileName(child);
        }

        qsort(fileNames.begin(), fileNames.length(), sizeof(Util::String), compareFileNames);

        Util::System::out << Util::String::join(" ", fileNames) << Util::Io::PrintStream::lnFlush;
    } else {
        Util::System::out << formatFileName(file) << Util::Io::PrintStream::lnFlush;
    }
}

int main(const int argc, char *argv[]) {
    Util::ArgumentParser argumentParser;
    argumentParser.setHelpText("List files.\n"
                               "Usage: ls [PATH]...\n"
                               "Options:\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    const auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        lsDirectory(Util::Io::File::getCurrentWorkingDirectory().getCanonicalPath());
    } else if (arguments.length() == 1) {
        lsDirectory(arguments[0]);
    } else {
        for (uint32_t i = 0; i < arguments.length(); i++) {
            Util::System::out << arguments[i] << ":" << Util::Io::PrintStream::ln;
            lsDirectory(arguments[i]);
            if (i < arguments.length() - 1) {
                Util::System::out << Util::Io::PrintStream::ln;
            }
        }
    }

    Util::System::out << Util::Io::PrintStream::flush;
    return 0;
}