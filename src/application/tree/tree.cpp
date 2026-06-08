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

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <util/graphic/Ansi.h>
#include <util/base/System.h>
#include <util/base/ArgumentParser.h>
#include <util/collection/Array.h>
#include <util/io/file/File.h>
#include <util/base/String.h>
#include <util/io/stream/PrintStream.h>

constexpr const char *HELP_TEXT =
#include "generated/README.md"
;

/// Comparison function for `qsort()` using pointers of the type `Util::Io::File`.
/// This is used to sort files alphabetically by their names.
int compareFileNames(const void *a, const void *b) {
    const auto &fileA = *static_cast<const Util::Io::File*>(a);
    const auto &fileB = *static_cast<const Util::Io::File*>(b);
    return strcmp(static_cast<const char*>(fileA.getName()), static_cast<const char*>(fileB.getName()));
}

/// Get the text color for a specific file type.
/// This is used to visually distinct, for example, folders and files.
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
    }

    return Util::Graphic::Ansi::FOREGROUND_WHITE;
}

/// Print the file at the given path with multiple '-' signs, depicting the current folder depth, to standard out.
/// If the file is a directory, this function calls itself recursively for all files in the directory.
/// Files are printed, sorted by their names.
/// ANSI escape sequences are used to color file names depending on their file type.
void treeDirectory(const Util::String &path, const size_t level) {
    const Util::Io::File file(path);
    if (!file.exists()) {
        Util::System::error << "tree: '" << path << "' not found!" << Util::Io::PrintStream::lnFlush;
        return;
    }

    auto string = Util::String("|-");
    for (size_t i = 0; i < level; i++) {
        string += "-";
    }

    string += getTypeColor(file) + file.getName() + (file.isDirectory() ? "/" : "") +
        Util::Graphic::Ansi::FOREGROUND_DEFAULT + " ";

    Util::System::out << string << Util::Io::PrintStream::lnFlush;

    if (file.isDirectory()) {
        const auto children = file.getChildren();
        qsort(children.begin(), children.length(), sizeof(Util::Io::File), compareFileNames);

        for (const auto &child : children) {
            treeDirectory(child.getCanonicalPath(), level + 1);
        }
    }
}

int32_t main(const int32_t argc, char *argv[]) {
    Util::ArgumentParser argumentParser;
    argumentParser.setHelpText("Print a directory tree.\n"
                               "Usage: tree [DIRECTORY]...\n"
                               "Options:\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        treeDirectory(Util::Io::File::getCurrentWorkingDirectory().getCanonicalPath(), 0);
    } else {
        for (size_t i = 0; i < arguments.length(); i++) {
            treeDirectory(arguments[i], 0);
        }
    }

    return 0;
}