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

#include <lib/util/base/System.h>
#include <lib/util/base/ArgumentParser.h>
#include <lib/util/collection/Array.h>
#include <lib/util/io/file/File.h>
#include <lib/util/base/String.h>
#include <lib/util/io/stream/FileInputStream.h>
#include <lib/util/io/stream/FileOutputStream.h>
#include <lib/util/io/stream/PrintStream.h>

const char *HELP_MESSAGE =
#include "generated/README.md"
;

uint8_t buffer[4096];

/// Copy data from the source file stream to target file stream.
/// The function reads data in chunks of 4096 bytes until a read operation returns 0 or less.
void copyFile(Util::Io::FileInputStream &source, Util::Io::FileOutputStream &target) {
    int32_t read;
    do {
        read = source.read(buffer, 0, 4096);
        if (read > 0) {
            target.write(buffer, 0, read);
        }
    } while (read > 0);
}

int main(const int argc, char *argv[]) {
    Util::ArgumentParser argumentParser;
    argumentParser.setHelpText(HELP_MESSAGE);

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() < 2) {
        Util::System::error << "cp: Missing arguments!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    if (arguments.length() == 2) {
        const auto sourceFile = Util::Io::File(arguments[0]);
        auto targetFile = Util::Io::File(arguments[1]);

        if (!sourceFile.exists()) {
            Util::System::error << "cp: '" << arguments[0] << "' not found!" << Util::Io::PrintStream::lnFlush;
            return -1;
        }

        if (!sourceFile.isFile()) {
            Util::System::error << "cp: '" << arguments[0] << "' is a directory!" << Util::Io::PrintStream::lnFlush;
            return -1;
        }

        if (sourceFile.getType() != Util::Io::File::REGULAR) {
            Util::System::error << "cp: '" << arguments[0] << "' is not a regular file!" << Util::Io::PrintStream::lnFlush;
            return -1;
        }

        if (targetFile.exists() && targetFile.isDirectory()) {
            targetFile = Util::Io::File(targetFile.getCanonicalPath() + "/" + sourceFile.getName());
        }

        if (!targetFile.exists() && !targetFile.create(Util::Io::File::REGULAR)) {
            Util::System::error << "cp: Failed to create file '" << arguments[1] << "'!" <<
                Util::Io::PrintStream::lnFlush;
            return -1;
        }

        Util::Io::FileInputStream source(sourceFile);
        Util::Io::FileOutputStream target(targetFile);
        copyFile(source, target);
    } else {
        const auto targetDirectory = Util::Io::File(arguments[arguments.length() - 1]);
        if (!targetDirectory.exists()) {
            Util::System::error << "cp: '" << arguments[arguments.length() - 1] << "' not found!" <<
                Util::Io::PrintStream::lnFlush;
        }

        if (!targetDirectory.isDirectory()) {
            Util::System::error << "cp: '" << arguments[arguments.length() - 1] << "' is not a directory!" <<
                Util::Io::PrintStream::lnFlush;
        }

        for (size_t i = 0; i < arguments.length() - 1; i++) {
            const auto sourceFile = Util::Io::File(arguments[i]);
            if (!sourceFile.exists()) {
                Util::System::error << "cp: '" << arguments[i] << "' not found!" << Util::Io::PrintStream::lnFlush;
                continue;
            }

            if (!sourceFile.isFile()) {
                Util::System::error << "cp: '" << arguments[i] << "' is a directory!" << Util::Io::PrintStream::lnFlush;
                continue;
            }

            if (sourceFile.getType() != Util::Io::File::REGULAR) {
                Util::System::error << "cp: '" << arguments[i] << "' is not a regular file!" <<
                    Util::Io::PrintStream::lnFlush;
                continue;
            }

            auto targetFile = Util::Io::File(targetDirectory.getCanonicalPath() + "/" + sourceFile.getName());

            if (!targetFile.exists() && !targetFile.create(Util::Io::File::REGULAR)) {
                Util::System::error << "cp: Failed to create file '" << targetFile.getCanonicalPath() << "'!" <<
                    Util::Io::PrintStream::lnFlush;
                continue;
            }

            Util::Io::FileInputStream source(sourceFile);
            Util::Io::FileOutputStream target(targetFile);
            copyFile(source, target);
        }
    }

    return 0;
}