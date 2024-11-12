/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <stdint.h>

#include "lib/util/base/System.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/collection/Array.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/String.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/stream/FileOutputStream.h"
#include "lib/util/io/stream/PrintStream.h"

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Copy a file from SOURCE to DESTINATION.\n"
                               "Usage: cp [SOURCE] [DESTINATION]\n"
                               "Options:\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() < 2) {
        Util::System::error << "cp: Missing arguments!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto sourceFile = Util::Io::File(arguments[0]);
    auto targetFile = Util::Io::File(arguments[1]);

    if (!sourceFile.exists()) {
        Util::System::error << "cp: '" << arguments[0] << "' not found!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    if (!sourceFile.isFile()) {
        Util::System::error << "cp: '" << arguments[0] << "' is a directory!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    if (sourceFile.getType() != Util::Io::File::REGULAR) {
        Util::System::error << "cp: '" << arguments[0] << "' is not a regular file!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    if (targetFile.exists() && targetFile.isDirectory()) {
        targetFile = Util::Io::File(targetFile.getCanonicalPath() + "/" + sourceFile.getName());
    }

    if (!targetFile.exists() && !targetFile.create(Util::Io::File::REGULAR)) {
        Util::System::error << "cp: Failed to create file '" << arguments[1] << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }
	
    auto source = Util::Io::FileInputStream(sourceFile);
    auto target = Util::Io::FileOutputStream(targetFile);

    auto *buffer = new uint8_t[4096];
    int32_t read;
    do {
        read = source.read(buffer, 0, 4096);
        if (read > 0) {
            target.write(buffer, 0, read);
        }
    } while (read != -1);

    delete[] buffer;
    return 0;
}